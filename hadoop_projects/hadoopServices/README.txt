This is how you login to nodes :

example : 
serup:hadoopServices$ vagrant -XY ssh one
Last login: Thu Aug 11 09:03:10 2016 from 10.0.2.2
Welcome to your Vagrant-built virtual machine.


[vagrant@one ~]$ hadoop fs -ls /
Found 4 items
drwxr-xr-x   - hdfs   hdfs          0 2016-08-08 12:48 /apps
drwx------   - mapred hdfs          0 2016-08-08 12:49 /mapred
drwxrwxrwx   - hdfs   hdfs          0 2016-08-11 10:22 /tmp
drwxr-xr-x   - hdfs   hdfs          0 2016-08-08 12:48 /user

[vagrant@one ~]$ cd /vagrant/
[vagrant@one vagrant]$ 
[vagrant@one vagrant]$ ll
total 84
drwx------ 1 vagrant vagrant 4096 May 18 11:19 ambari-provisioning
-rw-rw-r-- 1 vagrant vagrant  396 Jul  7 12:50 bootstrap-node.sh
-rw-rw-r-- 1 vagrant vagrant  414 Jul  7 13:54 bootstrap-server.sh
drwxr-xr-x 1 vagrant vagrant 4096 May 19 13:16 manifest
drwxr-xr-x 1 vagrant vagrant 4096 May 19 07:31 modules
-rw-rw-r-- 1 vagrant vagrant  541 Sep 30 06:43 README.txt
-rw-rw-r-- 1 vagrant vagrant  630 Jul  7 14:12 setup-repair-node-scripts.sh
-rw-rw-r-- 1 vagrant vagrant  123 Jul  7 14:12 tmpDeleteCorruptFiles.sh
-rw-rw-r-- 1 vagrant vagrant 1975 May 19 13:16 Vagrantfile

[vagrant@one vagrant]$ cat bootstrap-server.sh 
echo checking if server is running...
RED='\033[0;31m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color
GREEN='\033[0;32m'
ISOK=
if [  ==  ]; then
        echo -e STATUS: Server is NOT running, now start it
        sudo ambari-server start
        sudo ambari-agent start
else
        echo -e STATUS: Server is running
fi

[vagrant@one vagrant]$ bash bootstrap-server.sh 
checking if server is running...
STATUS: Server is running
[vagrant@one vagrant]$ 

[vagrant@one ~]$ exit
logout
Connection to 127.0.0.1 closed.



==============================
As you can see from above in this example the ambari is running
- I made a vagrant resume and this started the three nodes, I can 
  then go into online ambari administrator like this
   http://one.cluster:8080
   user: admin
   password: admin
   -- suggest changing user and password when you have a fully working cluster

