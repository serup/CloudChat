#!/bin/sh

# Run on VM to bootstrap Puppet Agent nodes

sudo rm -rf /var/lib/puppet/ssl/*

sudo cp /vagrant/sources.list /etc/apt/sources.list

if ps aux | grep "puppet agent" | grep -v grep 2> /dev/null
then
    echo "Puppet Agent is already installed. Moving on..."
else
    sudo cp /vagrant/sources.list /etc/apt/sources.list
    sudo apt-get update
    sudo apt-get install -yq puppet
fi

if cat /etc/puppet/puppet.conf | grep environment 2> /dev/null
then
    echo "Puppet Agent is already configured. "
    sudo service puppet stop
    sudo rm -rf /var/lib/puppet/ssl/*
else
    sudo apt-get update -yq && sudo apt-get upgrade -yq
    sudo rm -rf /var/lib/puppet/ssl/*

    # Configure /etc/hosts file
    sudo bash /vagrant/confighosts.sh

    # Add agent section to /etc/puppet/puppet.conf
    echo "" | sudo tee --append /etc/puppet/puppet.conf 2> /dev/null && \
    echo "[agent]" | sudo tee --append /etc/puppet/puppet.conf 2> /dev/null && \
    echo "    server=puppet" | sudo tee --append /etc/puppet/puppet.conf 2> /dev/null && \
    echo "    environment=devtest" | sudo tee --append /etc/puppet/puppet.conf 2> /dev/null
    sudo cp /vagrant/nodes.rc.local /etc/rc.local
    # bugfix hostname for ubuntu 15.04 - run this command    
    sudo bash /etc/init.d/hostname.sh
fi

    sudo puppet agent --enable

    echo "****************"
    echo "     INIT       "
    echo "****************"
    echo "fetch g++ since somehow gcc default install does not get it"
    apt-get install -yq g++
    echo "install sshpass to allow automatic transfer of images script to run - consider using ssh keys instead in future"
    apt-get install -yq sshpass
    echo "install incron to monitor event/changes in transfer folder "
    apt-get install -yq incron
    echo "root" >> /etc/incron.allow
    echo "root" >> /etc/cron.allow
    echo "vagrant" >> /etc/incron.allow
    echo "vagrant" >> /etc/cron.allow

    echo "****************"
    echo " taskwarrior "
    echo "****************"
    sudo apt-get install -yq taskwarrior    

    echo "****************"
    echo "Set up swapfile"
    echo "****************"
    echo "- Create Storage File"
    dd if=/dev/zero of=/swapfile1 bs=1024 count=524288
    echo "- Secure swap file"
    chown root:root /swapfile1
    chmod 0600 /swapfile1
    echo "- Set up Linux swap area"
    mkswap /swapfile1
    echo "- Enabling the swap file"
    swapon /swapfile1
    echo "- Update /etc/fstab file"
    echo "/swapfile1 none swap sw 0 0" >> /etc/fstab
    echo "- Result:"
    free -m

    echo "****************"
    echo "  HADOOP SETUP"
    echo "****************"
    XPRTJAVA=$(cat .bashrc | grep 'export JAVA_HOME')
    $(cat .bashrc | grep 'export JAVA_HOME')
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
    sed -i 's/export JAVA_HOME=${JAVA_HOME}/export JAVA_HOME=\/usr\/lib\/jvm\/java-8-oracle/' hadoop/etc/hadoop/hadoop-env.sh  
 
    echo "**********************************************************************************************************************"
    echo " example where grep is the Map/Reduce method and input folder is searched using regex, output is then in folder output"
    echo "**********************************************************************************************************************"
    sudo mkdir input
    sudo cp ./hadoop/etc/hadoop/*.xml input
    ./hadoop/bin/hadoop jar ./hadoop/share/hadoop/mapreduce/hadoop-mapreduce-examples-2.7.1.jar grep input output 'dfs[a-z.]+' 2> /dev/null
    cat output/* 

    echo "**********************************************************************************************************************"

    sudo ssh-keygen -t dsa -P '' -f ~/.ssh/id_dsa
    sudo cat ~/.ssh/id_dsa.pub >> ~/.ssh/authorized_keys
    export HADOOP\_PREFIX=/home/vagrant/hadoop-2.7.1

    echo "*******************************"
    echo " setup distributed file system " 
    echo "*******************************"
#    cd hadoop/bin
#    ./hdfs namenode -format
#    cd ..
#    cd sbin
#    ./start-dfs.sh
    
    sudo ./hadoop/sbin/stop-all.sh
    sudo ./hadoop/sbin/start-all.sh

    echo "*******************************"


