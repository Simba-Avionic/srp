#!/bin/bash
# QEMU tests of the EC (sim build) - functional + known-bug reproductions.
#   tools/qemu_test/qemu_test.sh [functional|bugs|all] [--no_cache]
#     functional : pytest on a QEMU guest in docker (tests/audit, marker functional); exit code 0 = green
#     bugs       : UT with tag audit_bugs (bazel) + pytest marker bugs on QEMU; every test is EXPECTED to be red
#                  (they reproduce open findings F-xx), the result is informational (exit code 0)
#     all        : both
#   --no_cache   : bazel --nocache_test_results and rebuild of the docker image
# Needs: bazel, docker (current user allowed to use it), /dev/net/tun; ~1 GB RAM per guest.
# Results: qemu_test_results/ (JUnit xml, logs, mapping.md for bugs).
set -u

MODE="functional"
NO_CACHE=0
for a in "$@"; do
  case "$a" in
    functional|bugs|all) MODE="$a" ;;
    --no_cache) NO_CACHE=1 ;;
    *) echo "usage: $0 [functional|bugs|all] [--no_cache]" >&2; exit 2 ;;
  esac
done

if ! command -v bazel &> /dev/null; then
    echo "Bazel is not installed"
    exit 1
fi
if ! command -v docker &> /dev/null || ! docker info &> /dev/null; then
    echo "Docker is not installed or the current user cannot use it"
    exit 1
fi
if [ ! -c /dev/net/tun ]; then
    echo "/dev/net/tun is missing (needed for the QEMU network in the container)"
    exit 1
fi

REPO="$(cd "$(dirname "$0")/../.." && pwd)"
cd "$REPO" || exit 1
OUT="$REPO/qemu_test_results"
mkdir -p "$OUT"
RUNNER="$REPO/tools/qemu_test/qemu_docker.sh"
BZFLAGS="${QEMU_TEST_BAZEL_FLAGS:-}"
CACHE_FLAG=""
[ "$NO_CACHE" = 1 ] && CACHE_FLAG="--nocache_test_results"

cleanup() {
  docker ps -q --filter label=srp-qemu-test=1 | xargs -r docker stop > /dev/null 2>&1
}
trap cleanup EXIT

echo "== build: EC package (sim) + QEMU image =="
bazel build $BZFLAGS --config=bbb-platform --define build_mode=sim \
  //deployment/cpu/ec:pkg //deployment/tools/ecu:flash_ec @srp_platform//tools/qemu || exit 1
if [ "$NO_CACHE" = 1 ] || ! docker image inspect "${QEMU_TEST_IMAGE:-srp-qemu-test:latest}" > /dev/null 2>&1; then
  echo "== build: docker image =="
  "$RUNNER" build || exit 1
fi

FUNC_RC=0
FUNC_SUMMARY=""
BUGS_SUMMARY=""

if [ "$MODE" = functional ] || [ "$MODE" = all ]; then
  echo "== functional: QEMU guest in docker =="
  START=$SECONDS
  "$RUNNER" run functional functional > "$OUT/functional.q.txt" 2>&1
  FUNC_RC=$?
  cp -f "$OUT/functional/functional.xml" "$OUT/functional.q.xml" 2> /dev/null
  tail -n 15 "$OUT/functional.q.txt"
  FUNC_SUMMARY="functional: rc=$FUNC_RC in $((SECONDS - START)) s ($(grep -E ' passed| failed' "$OUT/functional.q.txt" | tail -n 1))"
fi

if [ "$MODE" = bugs ] || [ "$MODE" = all ]; then
  echo "== bugs: unit tests with tag audit_bugs (expected RED) =="
  UT_TARGETS="$(bazel query 'attr(tags, "audit_bugs", kind(cc_test, //...))' 2> /dev/null) //mw/i2c_service/tests:adcsensor_test"
  # shellcheck disable=SC2086
  bazel test $BZFLAGS $CACHE_FLAG --keep_going --test_output=errors $UT_TARGETS > "$OUT/bugs.ut.log" 2>&1
  grep -E '^//.*(PASSED|FAILED)|Executed' "$OUT/bugs.ut.log"
  echo "== bugs: QEMU guest in docker (expected RED) =="
  "$RUNNER" run bugs bugs > "$OUT/bugs.q.txt" 2>&1
  cp -f "$OUT/bugs/bugs.xml" "$OUT/bugs.q.xml" 2> /dev/null
  python3 "$REPO/tools/qemu_test/make_mapping.py" "$REPO" > "$OUT/mapping.md"
  BUGS_SUMMARY="bugs (informational): $(grep -c 'RED (potwierdzone)' "$OUT/mapping.md") reproduced, $(grep -c 'PASS (nieoczekiwane' "$OUT/mapping.md") unexpectedly green (fixed?), $(grep -c 'FALSE ALARM' "$OUT/mapping.md") harness problems; see qemu_test_results/mapping.md"
fi

echo
echo "== summary =="
[ -n "$FUNC_SUMMARY" ] && echo "$FUNC_SUMMARY"
[ -n "$BUGS_SUMMARY" ] && echo "$BUGS_SUMMARY"
[ -n "${GITHUB_STEP_SUMMARY:-}" ] && { echo "### qemu_test ($MODE)"; echo "$FUNC_SUMMARY"; echo "$BUGS_SUMMARY"; } >> "$GITHUB_STEP_SUMMARY"
exit "$FUNC_RC"
