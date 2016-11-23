    echo "****************"
    echo "  HADOOP SETUP"
    echo "****************"
    XPRTJAVA=$(cat ~/.bashrc | grep 'export JAVA_HOME')
    $(cat ~/.bashrc | grep 'export JAVA_HOME')
    sudo wget -nc http://www.eu.apache.org/dist/hadoop/common/KEYS 2> /dev/null
    sudo wget -nc http://www.eu.apache.org/dist/hadoop/common/hadoop-2.7.1/hadoop-2.7.1.tar.gz.asc  2> /dev/null
    sudo wget -nc http://www.eu.apache.org/dist/hadoop/common/hadoop-2.7.1/hadoop-2.7.1.tar.gz  2> /dev/null
    sudo gpg --import KEYS  2> /dev/null
    sudo gpg --verify hadoop-2.7.1.tar.gz.asc hadoop-2.7.1.tar.gz  2> /dev/null
    sudo tar -xvf hadoop-2.7.1.tar.gz | nl | tail -1 | awk '{print "lines extracted:"  $1}'
    cd hadoop-2.7.1/bin
    ./hadoop version|grep Hadoop | awk '{print $2}'
    cd ../..
    sudo mv hadoop-2.7.1 hadoop
    sudo sed -i 's/export JAVA_HOME=${JAVA_HOME}/export JAVA_HOME=\/usr\/lib\/jvm\/java-8-oracle/' hadoop/etc/hadoop/hadoop-env.sh
    sudo mv hadoop /usr/local/.

    sudo mkdir -p /usr/local/hadoop/hadoop_data/hdfs/namenode
    sudo mkdir -p /usr/local/hadoop/hadoop_data/hdfs/datanode


echo "shold be put in ~/.bashrc file :"
#Hadoop Variables
          export JAVA_HOME=/usr/lib/jvm/java-8-oracle
          export HADOOP_HOME=/usr/local/hadoop
          export PATH=$PATH:$HADOOP_HOME/bin
          export PATH=$PATH:$HADOOP_HOME/sbin
          export HADOOP_MAPRED_HOME=$HADOOP_HOME
          export HADOOP_COMMON_HOME=$HADOOP_HOME
          export HADOOP_HDFS_HOME=$HADOOP_HOME
          export YARN_HOME=$HADOOP_HOME
          export HADOOP_COMMON_LIB_NATIVE_DIR=$HADOOP_HOME/lib/native
          export HADOOP_OPTS="-Djava.library.path=$HADOOP_HOME/lib"

    # hdfs namenode -format
    # start-all.sh

    jps

    echo "**********************************************************************************************************************"
    echo " example where grep is the Map/Reduce method and input folder is searched using regex, output is then in folder output"
    echo "**********************************************************************************************************************"
#    sudo mkdir input
#    sudo cp ./hadoop/etc/hadoop/*.xml input
#    ./hadoop/bin/hadoop jar ./hadoop/share/hadoop/mapreduce/hadoop-mapreduce-examples-2.7.1.jar grep input output 'dfs[a-z.]+' 2> /dev/null
#    cat output/*

    echo "**********************************************************************************************************************"

#    sudo ssh-keygen -t dsa -P '' -f ~/.ssh/id_dsa
#    sudo cat ~/.ssh/id_dsa.pub >> ~/.ssh/authorized_keys
#    export HADOOP\_PREFIX=/home/vagrant/hadoop-2.7.1


