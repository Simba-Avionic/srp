#!/bin/bash
# Runs INSIDE the audit container: tap0 -> qemu (same args as @srp_platform//tools/qemu run_script.sh) -> alias .51
# -> flash (flash_script.sh + pkg.tar) -> pytest / shell / cmd.   Usage: entry.sh <functional|bugs|shell|cmd...>
# Mounts (see qemu_docker.sh): /img (RO qemu image), /pkg (RO pkg.tar + flash_script.sh), /repo/{tests,tools}/audit (RO),
# /sim (RO srp-simulations), /results (RW).
set -u
RES=/results
mkdir -p "$RES/logs"
GUEST_IP=192.168.10.51
FRESH_IP=192.168.10.101
SSH="ssh -o BatchMode=yes -o StrictHostKeyChecking=no -o UserKnownHostsFile=/dev/null -o LogLevel=ERROR -o ConnectTimeout=2"
log() { echo "[entry $(date +%T)] $*" | tee -a "$RES/logs/entry.log"; }

ip tuntap add dev tap0 mode tap || { log "cannot create tap0 (need --cap-add NET_ADMIN --device /dev/net/tun)"; exit 10; }
ip addr add 192.168.10.4/24 dev tap0
ip link set tap0 up

IMG=/img/disk.ext4
KERNEL=/img/zImage
log "starting qemu: $IMG"
setsid qemu-system-arm -device virtio-net-device,netdev=net0,mac=52:54:00:12:34:02 \
  -netdev tap,id=net0,ifname=tap0,script=no,downscript=no -object rng-random,filename=/dev/urandom,id=rng0 \
  -device virtio-rng-pci,rng=rng0 -drive id=disk0,file="$IMG",if=none,format=raw,snapshot=on \
  -device virtio-blk-device,drive=disk0 -device qemu-xhci -device usb-tablet -device usb-kbd \
  -machine virt,highmem=off -cpu cortex-a15 -smp 4 -m 256 -accel tcg,thread=multi,tb-size=256 -nographic \
  -kernel "$KERNEL" \
  -append 'root=/dev/vda rw  mem=256M ip=192.168.7.2::192.168.7.1:255.255.255.0::eth0:off:8.8.8.8 net.ifnames=0 swiotlb=0 ' \
  < /dev/null > "$RES/logs/qemu.log" 2>&1 &
QPID=$!

ok=0
for _ in $(seq 1 120); do
  kill -0 $QPID 2>/dev/null || { log "qemu exited early, see logs/qemu.log"; exit 11; }
  if $SSH root@$GUEST_IP true 2>/dev/null; then ok=1; break; fi
  if $SSH root@$FRESH_IP true 2>/dev/null; then
    $SSH root@$FRESH_IP "ip addr add $GUEST_IP/24 dev eth0" 2>/dev/null
    $SSH root@$GUEST_IP true 2>/dev/null && { ok=1; break; }
  fi
  sleep 3
done
[ $ok = 1 ] || { log "guest never answered on ssh"; exit 12; }
log "guest reachable at $GUEST_IP"

if [ -f /pkg/pkg.tar ]; then
  mkdir -p /tmp/flash/deployment/cpu/ec && cp /pkg/pkg.tar /tmp/flash/deployment/cpu/ec/pkg.tar
  (cd /tmp/flash && sh /pkg/flash_script.sh) > "$RES/logs/flash.log" 2>&1 || { log "flash failed, see logs/flash.log"; exit 13; }
  log "flashed, guest rebooting"
fi

export QEMU_TEST_IN_CONTAINER=1 QEMU_TEST_WORK="$RES" PYTHONDONTWRITEBYTECODE=1
T="${1:-functional}"
case "$T" in
  functional|bugs)
    cd /repo/tests/audit
    pytest -m "$T" -v -p no:cacheprovider --junitxml="$RES/$T.xml" 2>&1 | tee "$RES/$T.txt"
    rc=${PIPESTATUS[0]}
    ;;
  shell) bash; rc=$? ;;
  *) "$@"; rc=$? ;;
esac
kill $QPID 2>/dev/null; chown -R "${HOST_UID:-0}:${HOST_GID:-0}" "$RES" 2>/dev/null
log "done rc=$rc"
exit $rc
