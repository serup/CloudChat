echo "yum upgrade -- please wait..."
sudo yum -y clean all
sudo yum -y check 
sudo yum -y upgrade 
echo "install java..."
sudo yum -y install java

PLATFORM=$(uname -i)
if [ "x86_64" != $PLATFORM ]; then
export JAVA_HOME=/usr/jdk/jdk1.6.0_31/bin
else
export JAVA_HOME=/usr/jdk64/jdk1.6.0_31/bin
fi

export PATH=${JAVA_HOME}/bin:${PATH}
export HADOOP_CLASSPATH=${JAVA_HOME}/lib/tools.jar

PKG_OK=$(sudo yum repolist|grep ambari)
if [ "" == "$PKG_OK" ]; then
        echo "WARNING - puppet should have installed - now use script instead"
	#sudo wget http://public-repo-1.hortonworks.com/ambari/centos6/1.x/GA/ambari.repo
	sudo wget http://public-repo-1.hortonworks.com/ambari/centos6/1.x/updates/1.2.3.7/ambari.repo
	sudo cp ambari.repo /etc/yum.repos.d
        #sudo sed -i 's/https/http/g' /etc/yum.repos.d/epel.repo
	sudo yum -y install ambari-server
	sudo ambari-server setup
	sudo yum -y install ambari-agent
fi

echo "start ambari server"
sudo ambari-server start
echo "start ambari agent"
sudo ambari-agent start
