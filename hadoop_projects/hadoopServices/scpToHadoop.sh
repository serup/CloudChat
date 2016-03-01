#!/bin/bash
source="$1"
#destination="$2"

#example : sshpass -p "vagrant" scp -oStrictHostKeyChecking=no testscp.txt vagrant@one.cluster:/home/vagrant/.
#sshpass -p "vagrant" scp -oStrictHostKeyChecking=no $source vagrant@one.cluster:/home/vagrant/$destination/.
sshpass -p "vagrant" scp -oStrictHostKeyChecking=no $source vagrant@one.cluster:/home/vagrant/.

