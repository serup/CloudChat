PLATFORM=$(uname -i)
if [ "x86_64" != $PLATFORM ]; then
echo "yum upgrade -- please wait..."
sudo yum -y clean all
sudo yum -y check 
sudo yum -y upgrade 
fi
echo "start ambari agent"
sudo ambari-agent start
