#!/bin/bash
# Checks that all EC apps run on the QEMU guest and stay up.
# Usage: check_apps.sh [seconds=60]
# Exit 0 = every expected process present at start, same PIDs after N seconds,
# and no crash markers in the console log since the last boot.
set -u
WORK="${QEMU_TEST_WORK:-/tmp/qemu_test}"
GUEST_IP="${GUEST_IP:-192.168.10.51}"
SECS="${1:-60}"
LOG="$WORK/logs/qemu.log"
SSH="ssh -o BatchMode=yes -o StrictHostKeyChecking=no -o UserKnownHostsFile=/dev/null -o LogLevel=ERROR -o ConnectTimeout=3"
# comm names (kernel truncates to 15 chars): SIMBA_COMPUTER_BASE_APPS + deployment/cpu/ec/BUILD
EXPECTED="${EXTRA_APPS:-} em state_manager diag_demon logger someip_demon timestamp_servi i2cMWService gpio_service temp_service servoService EngineService EnvApp FileLoggerApp SysStatService PrimerService"

snapshot() {  # prints "comm pid" for expected processes
  $SSH "root@$GUEST_IP" ps 2>/dev/null | awk '{print $4, $1}' | while read -r c p; do
    for e in $EXPECTED; do [ "$c" = "$e" ] && echo "$c $p"; done
  done | sort
}

missing() {  # $1 = snapshot
  for e in $EXPECTED; do echo "$1" | grep -q "^$e " || echo -n "$e "; done
}

end=$((SECONDS + 90))
S0=""
while [ $SECONDS -lt $end ]; do
  S0="$(snapshot)"
  [ -z "$(missing "$S0")" ] && break
  sleep 3
done
M="$(missing "$S0")"
if [ -n "$M" ]; then
  echo "FAIL: missing processes: $M"
  exit 1
fi
sleep "$SECS"
S1="$(snapshot)"
if [ "$S0" != "$S1" ]; then
  echo "FAIL: process set changed after ${SECS}s"
  diff <(echo "$S0") <(echo "$S1")
  exit 1
fi
# console log since the last kernel banner
BAD=""
if [ -f "$LOG" ]; then
  start="$(grep -n 'Linux version' "$LOG" | tail -1 | cut -d: -f1)"
  BAD="$(tail -n +"${start:-1}" "$LOG" | grep -a -E 'terminate called|Segmentation fault|core dumped|Aborted|FATAL|std::bad_' | head -5)"
fi
if [ -n "$BAD" ]; then
  echo "FAIL: crash markers in console log:"; echo "$BAD"
  exit 1
fi
echo "PASS: $(echo "$S0" | wc -l) processes alive and stable for ${SECS}s, no crash markers"
exit 0
