#!/usr/bin bash -l
sshpass -p "vagrant" scp -oStrictHostKeyChecking=no -rp bootstrap-server.sh vagrant@one.cluster:/home/vagrant/.
sshpass -p "vagrant" scp -oStrictHostKeyChecking=no -rp bootstrap-node.sh vagrant@two.cluster:/home/vagrant/.
sshpass -p "vagrant" scp -oStrictHostKeyChecking=no -rp bootstrap-node.sh vagrant@three.cluster:/home/vagrant/.

echo 'sudo -su hdfs
hadoop dfsadmin -safemode leave
hadoop fs -rmr -skipTrash /tmp/input/findprofile
hadoop fsck -files delete /' > tmpDeleteCorruptFiles.sh

sshpass -p "vagrant" scp -oStrictHostKeyChecking=no -rp tmpDeleteCorruptFiles.sh vagrant@one.cluster:/home/vagrant/.

