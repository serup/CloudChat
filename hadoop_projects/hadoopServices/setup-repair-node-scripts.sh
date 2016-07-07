#!/usr/bin bash -l
sshpass -p "vagrant" scp -oStrictHostKeyChecking=no -rp bootstrap-server.sh vagrant@one.cluster:/home/vagrant/.
sshpass -p "vagrant" scp -oStrictHostKeyChecking=no -rp bootstrap-node.sh vagrant@two.cluster:/home/vagrant/.
sshpass -p "vagrant" scp -oStrictHostKeyChecking=no -rp bootstrap-node.sh vagrant@three.cluster:/home/vagrant/.
