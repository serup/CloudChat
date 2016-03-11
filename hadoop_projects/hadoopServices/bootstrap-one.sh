PLATFORM=$(uname -i)
if [ "x86_64" != $PLATFORM ]; then
echo "yum upgrade -- please wait..."
sudo yum -y clean all
sudo yum -y check 
sudo yum -y upgrade 
echo "install java..."
sudo yum -y install java

#sudo wget http://public-repo-1.hortonworks.com/ambari/centos6/1.x/GA/ambari.repo
#sudo cp ambari.repo /etc/yum.repos.d
#sudo yum repolist
#sudo sed -i 's/https/http/g' /etc/yum.repos.d/epel.repo
fi

echo "start ambari server"
sudo ambari-server start
echo "start ambari agent"
sudo ambari-agent start
