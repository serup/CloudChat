PLATFORM=$(uname -i)
if [ "x86_64" != $PLATFORM ]; then
echo "yum upgrade -- please wait..."
sudo yum -y clean all
sudo yum -y check 
sudo yum -y upgrade 
echo "install java..."
#sudo yum -y install java
#export JAVA_HOME=/usr/jdk/jdk1.6.0_31/bin
sudo yum install java-1.8.0-openjdk-devel
export JAVA_HOME=/usr/lib/jvm/java-1.8.0/bin

else

#export JAVA_HOME=/usr/jdk64/jdk1.6.0_31/bin
export JAVA_HOME=/usr/lib/jvm/java-1.8.0/bin

fi

export PATH=${JAVA_HOME}/bin:${PATH}
export HADOOP_CLASSPATH=${JAVA_HOME}/lib/tools.jar

echo "start ambari agent"
sudo ambari-agent start
