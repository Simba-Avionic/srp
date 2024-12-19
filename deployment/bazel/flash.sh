#!/bin/bash 
now=`date '+%Y_%m_%d_%H_%M_%S'`

ssh -o StrictHostKeyChecking=no -o UserKnownHostsFile=/dev/null $2 "rm -rf /srp/opt/*"
scp -o StrictHostKeyChecking=no -o UserKnownHostsFile=/dev/null $3 "$2:/srp/update/new/$4-$now.tar"


ssh -o StrictHostKeyChecking=no -o UserKnownHostsFile=/dev/null $2 "tar -xvf /srp/update/new/$4-$now.tar -C /srp"
ssh -o StrictHostKeyChecking=no -o UserKnownHostsFile=/dev/null $2 " sleep 2 & rm -rf /srp/update/new/$4-$now.tar"
timeout 2 ssh -o StrictHostKeyChecking=no -o UserKnownHostsFile=/dev/null -o ConnectTimeout=2  $2 " sleep 2 & reboot"