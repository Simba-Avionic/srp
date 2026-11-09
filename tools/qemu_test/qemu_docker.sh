#!/bin/bash
# Run a QEMU guest + tests inside a docker container (own netns => identical IPs in every container).
#   tools/qemu_test/qemu_docker.sh build
#   tools/qemu_test/qemu_docker.sh run <name> <functional|bugs|shell|cmd...>
#   tools/qemu_test/qemu_docker.sh ls | stop <name>
# Limits: max 3 containers at once, 1 GB each (QEMU_TEST_MEMORY). Results: qemu_test_results/<name>/ (junit xml, txt, logs/).
# Needs a finished sim build on the host: bazel-bin/deployment/cpu/ec/pkg.tar (flash_ec) and the qemu image
# (bazel-bin/external/srp_platform/tools/qemu, i.e. `bazel build @srp_platform//tools/qemu`).
set -u
REPO="$(cd "$(dirname "$0")/../.." && pwd)"
WORK="${QEMU_TEST_WORK:-/tmp/qemu_test}"
IMAGE="${QEMU_TEST_IMAGE:-srp-qemu-test:latest}"
MAX="${QEMU_TEST_MAX_CONTAINERS:-3}"
# 768 MB konczylo sie OOM-kill QEMU przy rebocie goscia (szczyt = limit), stad 1 GB
MEM="${QEMU_TEST_MEMORY:-1024m}"
cmd="${1:-}"; shift || true
case "$cmd" in
  build)
    exec docker build -t "$IMAGE" "$REPO/tools/qemu_test/docker" ;;
  ls)
    exec docker ps --filter label=srp-qemu-test=1 --format '{{.Names}}\t{{.Status}}' ;;
  stop)
    exec docker stop "srp-qemu-test-$1" ;;
  run) ;;
  *) echo "usage: $0 build | run <name> <functional|bugs|shell|cmd...> | ls | stop <name>" >&2; exit 2 ;;
esac

NAME="${1:?name required}"; shift
[[ "$NAME" =~ ^[A-Za-z0-9_-]+$ ]] || { echo "invalid run name: $NAME" >&2; exit 2; }
[ $# -ge 1 ] || set -- functional
RUNNING=$(docker ps -q --filter label=srp-qemu-test=1 | wc -l)
if [ "$RUNNING" -ge "$MAX" ]; then echo "refusing: $RUNNING containers already running (max $MAX)" >&2; exit 3; fi
docker image inspect "$IMAGE" >/dev/null 2>&1 || { echo "image $IMAGE missing: run '$0 build'" >&2; exit 4; }

ART="${QEMU_TEST_ARTIFACTS_REPO:-$REPO}"
# bazel-bin points to the output dir of the LAST bazel configuration; qemu_test.sh passes the sim one explicitly
BIN="${QEMU_TEST_BAZEL_BIN:-$ART/bazel-bin}"
QRUN="$BIN/external/srp_platform/tools/qemu/qemu.runfiles/srp/external/qemu_image"
DISK="$(readlink -f "$(ls "$QRUN"/*.ext4 | head -1)")"
KERN="$(readlink -f "$(ls "$QRUN"/zImage* | head -1)")"
PKG="$(readlink -f "$BIN/deployment/cpu/ec/pkg.tar")"
FLASH="$(readlink -f "$BIN/deployment/tools/ecu/flash_script.sh")"
for p in "$DISK" "$KERN" "$PKG" "$FLASH"; do [ -e "$p" ] || { echo "missing build artefact: $p" >&2; exit 5; }; done
OUT="$REPO/qemu_test_results/$NAME"
mkdir -p "$REPO/qemu_test_results"
docker run --rm -v "$REPO/qemu_test_results":/r "$IMAGE" sh -c "rm -rf /r/$NAME" 2>/dev/null; mkdir -p "$OUT"
SIMMOUNT=""; [ -n "${QEMU_TEST_SIMULATIONS:-}" ] && [ -d "$QEMU_TEST_SIMULATIONS" ] && SIMMOUNT="-v $QEMU_TEST_SIMULATIONS:/sim:ro"

exec docker run --rm --name "srp-qemu-test-$NAME" --label srp-qemu-test=1 \
  -e HOST_UID=$(id -u) -e HOST_GID=$(id -g) --memory "$MEM" --memory-swap "$MEM" --cap-add NET_ADMIN --device /dev/net/tun \
  -v "$DISK":/img/disk.ext4:ro -v "$KERN":/img/zImage:ro \
  -v "$PKG":/pkg/pkg.tar:ro -v "$FLASH":/pkg/flash_script.sh:ro \
  -v "$REPO/tests/audit":/repo/tests/audit:ro -v "$REPO/tools/qemu_test":/repo/tools/qemu_test:ro \
  -v "$REPO/deployment/apps/servo_service/config.json":/repo/deployment/apps/servo_service/config.json:ro \
  $SIMMOUNT \
  -v "$OUT":/results \
  "$IMAGE" /repo/tools/qemu_test/docker/entry.sh "$@"
