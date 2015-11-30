# "MUST BE STARTED AS ROOT !!!!"
# start scanvaserver and DFD1
touch /var/www/serverlog.txt
touch /var/www/databaselog.txt
curdir=/home/vagrant/CloudChat/codeblocks_projects
cd $curdir/websocket_server/bin/Debug
./scanvaserver 0.0.0.0 7777 > /var/www/serverlog.txt &  echo $! > /var/tmp/runningServerProcessID
sleep 10
cd $curdir/DFDFunctions/1_1/bin/Debug
./1_1 127.0.0.1 7777 > /var/www/databaselog.txt & echo $! >> /var/tmp/runningServerProcessID
