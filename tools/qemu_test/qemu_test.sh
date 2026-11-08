#!/bin/bash
# QEMU tests of the EC (sim build) - functional + known-bug reproductions, split into stages.
#   tools/qemu_test/qemu_test.sh [functional|bugs|all] [--no_cache]   all stages in a row (build, image, run, report)
#   tools/qemu_test/qemu_test.sh build                                bazel build of the EC sim package + QEMU image, staged
#   tools/qemu_test/qemu_test.sh image [--no_cache]                   docker image of the test container
#   tools/qemu_test/qemu_test.sh run <functional|bugs|all> [--no-build] [--no_cache]
#                                                                     guest + flash + tests; without --no-build it
#                                                                     runs build and image first
#   tools/qemu_test/qemu_test.sh report                               Markdown summary of the results on stdout
#   tools/qemu_test/qemu_test.sh cleanup                              stop containers, remove staged artefacts
#     functional : pytest on a QEMU guest in docker (tests/audit, marker functional); exit code 0 = green
#     bugs       : UT with tag audit_bugs (bazel) + pytest marker bugs on QEMU; every test is EXPECTED to be red
#                  (they reproduce open findings F-xx), the result is informational (exit code 0)
#     all        : both
#   --no_cache   : bazel --nocache_test_results and rebuild of the docker image
# Stages share state through qemu_test_results/ (*.status files) and a temp stage dir. The id is QEMU_TEST_RUN_ID;
# the one-command form uses a unique id per process (parallel runs do not collide), separate stages default to "ci".
# Needs: bazel, docker (current user allowed to use it), /dev/net/tun; ~1 GB RAM per guest.
# Results: qemu_test_results/ (JUnit xml, logs, mapping.md for bugs).
set -u

usage() {
  echo "usage: $0 [functional|bugs|all] [--no_cache] | build | image [--no_cache] |" >&2
  echo "       run <functional|bugs|all> [--no-build] [--no_cache] | report | cleanup" >&2
  exit 2
}

REPO="$(cd "$(dirname "$0")/../.." && pwd)"
cd "$REPO" || exit 1
OUT="$REPO/qemu_test_results"
RUNNER="$REPO/tools/qemu_test/qemu_docker.sh"
BZFLAGS="${QEMU_TEST_BAZEL_FLAGS:-}"
IMAGE="${QEMU_TEST_IMAGE:-srp-qemu-test:latest}"
QIMG="external/srp_platform/tools/qemu/qemu.runfiles/srp/external/qemu_image"

STAGE_CMD="${1:-functional}"
MODE=""
case "$STAGE_CMD" in
  functional | bugs | all) CHAIN=1; MODE="$STAGE_CMD"; shift || true ;;
  build | image | run | report | cleanup) CHAIN=0; shift ;;
  *) usage ;;
esac

NO_CACHE=0
NO_BUILD=0
for a in "$@"; do
  case "$a" in
    functional | bugs | all)
      if [ "$STAGE_CMD" = run ] && [ -z "$MODE" ]; then MODE="$a"; else usage; fi ;;
    --no_cache) NO_CACHE=1 ;;
    --no-build)
      if [ "$STAGE_CMD" = run ]; then NO_BUILD=1; else usage; fi ;;
    *) usage ;;
  esac
done
if [ "$STAGE_CMD" = run ] && [ -z "$MODE" ]; then usage; fi

if [ "$CHAIN" = 1 ]; then
  RUN_ID="${QEMU_TEST_RUN_ID:-$$}"
else
  RUN_ID="${QEMU_TEST_RUN_ID:-ci}"
fi
STAGE="${TMPDIR:-/tmp}/qemu_test_stage-$RUN_ID"   # outside qemu_test_results/ (uploaded as artifact)
CACHE_FLAG=""
[ "$NO_CACHE" = 1 ] && CACHE_FLAG="--nocache_test_results"
mkdir -p "$OUT"

die() { echo "ERROR: $*" >&2; exit 1; }

stop_containers() {
  local n
  for n in "functional-$RUN_ID" "bugs-$RUN_ID"; do
    docker stop "srp-qemu-test-$n" > /dev/null 2>&1
  done
}

cleanup() {
  stop_containers
  rm -rf "$STAGE"
}

need_bazel() {
  command -v bazel &> /dev/null || die "Bazel is not installed"
}

need_docker() {
  if ! command -v docker &> /dev/null || ! docker info &> /dev/null; then
    die "Docker is not installed or the current user cannot use it"
  fi
  [ -c /dev/net/tun ] || die "/dev/net/tun is missing (needed for the QEMU network in the container)"
}

stage_ok() {
  [ -s "$STAGE/deployment/cpu/ec/pkg.tar" ] && [ -s "$STAGE/deployment/tools/ecu/flash_script.sh" ] &&
    [ -e "$STAGE/$QIMG" ]
}

do_build() {
  need_bazel
  echo "== build: EC package (sim) + QEMU image =="
  # shellcheck disable=SC2086
  bazel build $BZFLAGS --config=bbb-platform --define build_mode=sim \
    //deployment/cpu/ec:pkg //deployment/tools/ecu:flash_ec @srp_platform//tools/qemu || die "bazel build failed"
  # sim and sprzet builds share one bazel-bin dir (the --define does not change it), so another bazel command could
  # overwrite pkg.tar between the build and the guest start: stage the artifacts right after the build
  local bin
  # shellcheck disable=SC2086
  bin="$(bazel info $BZFLAGS --config=bbb-platform --define build_mode=sim bazel-bin 2> /dev/null)" ||
    die "bazel info bazel-bin failed"
  rm -rf "$STAGE"
  mkdir -p "$STAGE/deployment/cpu/ec" "$STAGE/deployment/tools/ecu" "$STAGE/$(dirname "$QIMG")"
  cp -L "$bin/deployment/cpu/ec/pkg.tar" "$STAGE/deployment/cpu/ec/pkg.tar" || die "missing pkg.tar"
  cp -L "$bin/deployment/tools/ecu/flash_script.sh" "$STAGE/deployment/tools/ecu/flash_script.sh" ||
    die "missing flash_script.sh"
  # the QEMU image does not depend on the sim define
  ln -s "$(readlink -f "$bin/$QIMG")" "$STAGE/$QIMG" || die "missing QEMU image"
  echo "staged artefacts in $STAGE"
}

do_image() {
  need_docker
  if [ "$NO_CACHE" = 1 ] || ! docker image inspect "$IMAGE" > /dev/null 2>&1; then
    echo "== image: docker test container =="
    "$RUNNER" build || die "docker image build failed"
  else
    echo "== image: $IMAGE already present (use --no_cache to rebuild) =="
  fi
}

# status file: "<rc> <seconds>" of the last run of a suite
write_status() { echo "$2 $3" > "$OUT/$1.status"; }

do_run() {
  need_docker
  if [ "$NO_BUILD" = 0 ] && [ "$CHAIN" = 0 ]; then
    do_build
    do_image
  fi
  stage_ok || die "no staged artefacts in $STAGE - run '$0 build' first (or drop --no-build)"
  docker image inspect "$IMAGE" > /dev/null 2>&1 || die "docker image $IMAGE missing - run '$0 image' first"
  export QEMU_TEST_BAZEL_BIN="$STAGE"
  local rc=0 start targets

  if [ "$MODE" = functional ] || [ "$MODE" = all ]; then
    echo "== functional: QEMU guest in docker =="
    rm -f "$OUT/functional.status"
    start=$SECONDS
    "$RUNNER" run "functional-$RUN_ID" functional > "$OUT/functional.q.txt" 2>&1
    rc=$?
    cp -f "$OUT/functional-$RUN_ID/functional.xml" "$OUT/functional.q.xml" 2> /dev/null
    tail -n 15 "$OUT/functional.q.txt"
    write_status functional "$rc" $((SECONDS - start))
  fi

  if [ "$MODE" = bugs ] || [ "$MODE" = all ]; then
    need_bazel
    echo "== bugs: unit tests with tag audit_bugs (expected RED) =="
    rm -f "$OUT/bugs.status"
    start=$SECONDS
    targets="$(bazel query 'attr(tags, "audit_bugs", kind(cc_test, //...))' 2> /dev/null)"
    # shellcheck disable=SC2086
    bazel test $BZFLAGS $CACHE_FLAG --keep_going --test_output=errors $targets > "$OUT/bugs.ut.log" 2>&1
    grep -E '^//.*(PASSED|FAILED)|Executed' "$OUT/bugs.ut.log"
    echo "== bugs: QEMU guest in docker (expected RED) =="
    "$RUNNER" run "bugs-$RUN_ID" bugs > "$OUT/bugs.q.txt" 2>&1
    cp -f "$OUT/bugs-$RUN_ID/bugs.xml" "$OUT/bugs.q.xml" 2> /dev/null
    python3 "$REPO/tools/qemu_test/make_mapping.py" "$REPO" > "$OUT/mapping.md"
    write_status bugs 0 $((SECONDS - start))   # informational: red is expected
  fi
  # exit code: functional result only (bugs are informational)
  return "$rc"
}

do_report() {
  local found=0 rc secs
  echo "### qemu_test"
  if [ -f "$OUT/functional.status" ]; then
    found=1
    read -r rc secs < "$OUT/functional.status"
    echo "- functional: rc=$rc in ${secs} s ($(grep -E ' passed| failed' "$OUT/functional.q.txt" | tail -n 1))"
  fi
  if [ -f "$OUT/bugs.status" ]; then
    found=1
    read -r _ secs < "$OUT/bugs.status"
    echo "- bugs (informational) in ${secs} s: $(grep -c 'RED (potwierdzone)' "$OUT/mapping.md") reproduced," \
      "$(grep -c 'PASS (nieoczekiwane' "$OUT/mapping.md") unexpectedly green (fixed?)," \
      "$(grep -c 'FALSE ALARM' "$OUT/mapping.md") harness problems"
  fi
  if [ "$found" = 0 ]; then
    echo "- no results in $OUT - the run stage did not execute"
    return 1
  fi
  if [ -f "$OUT/functional.status" ] && [ -f "$OUT/functional.q.txt" ]; then
    echo; echo "#### functional (tail)"; echo '```'; tail -n 40 "$OUT/functional.q.txt"; echo '```'
  fi
  if [ -f "$OUT/bugs.status" ] && [ -f "$OUT/mapping.md" ]; then
    echo; echo "#### bugs: F-xx mapping (red = bug reproduced)"; cat "$OUT/mapping.md"
  fi
  return 0
}

case "$STAGE_CMD" in
  build) do_build ;;
  image) do_image ;;
  run)
    # separate stages: only stop containers on exit, the staged artefacts stay for later steps / reruns
    trap stop_containers EXIT
    do_run
    ;;
  report) do_report ;;
  cleanup) cleanup; echo "cleaned up run $RUN_ID" ;;
  *)
    trap cleanup EXIT
    do_build
    do_image
    do_run
    rc=$?
    echo; echo "== summary =="
    do_report | grep '^- '
    [ -n "${GITHUB_STEP_SUMMARY:-}" ] && do_report >> "$GITHUB_STEP_SUMMARY"
    exit "$rc"
    ;;
esac
