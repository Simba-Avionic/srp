#!/bin/bash 
now=`date '+%Y_%m_%d_%H_%M_%S'`
echo $now
ssh -o StrictHostKeyChecking=no -o UserKnownHostsFile=/dev/null $2 "rm -rf /opt/*"
scp -o StrictHostKeyChecking=no -o UserKnownHostsFile=/dev/null $3 "$2:/etc/simba/new/$4-$now.tar"


ssh -o StrictHostKeyChecking=no -o UserKnownHostsFile=/dev/null $2 "tar -xvf /etc/simba/new/$4-$now.tar -C /"
ssh -o StrictHostKeyChecking=no -o UserKnownHostsFile=/dev/null $2 " sleep 2 & rm -rf /etc/simba/new/$4-$now.tar"
timeout 5 ssh -o StrictHostKeyChecking=no -o UserKnownHostsFile=/dev/null -o ConnectTimeout=10  $2 " sleep 2 & reboot"