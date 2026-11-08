#!/bin/bash
# Uzycie: flash_sim.sh <user@ip> [timeout_s]
# Po flash_ec (ktory robi reboot) czeka az gosc QEMU znow odpowie na ssh.
# Kod wyjscia 0 = gosc wstal, !=0 = timeout.
target="${1:?user@ip}"; limit="${2:-180}"
ssh_opts="-o BatchMode=yes -o StrictHostKeyChecking=no -o UserKnownHostsFile=/dev/null -o ConnectTimeout=2"
sleep 5   # flash_ec zleca reboot z opoznieniem 2 s
end=$((SECONDS+limit))
while [ $SECONDS -lt $end ]; do
  ssh $ssh_opts "$target" true 2>/dev/null && { echo "flash_sim: $target up"; exit 0; }
  sleep 3
done
echo "flash_sim: timeout waiting for $target" >&2; exit 1
