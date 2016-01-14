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
    echo "Puppet Agent is already configured. Exiting..."
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


    sudo puppet agent --enable

    echo "Fetch latest version of CloudChat"
    if [ -d "CloudChat" ]; then
      echo "CloudChat already installed"
      echo "updating ..."
      cd CloudChat
      git checkout serup
      git pull
    else
      echo "****************"
      echo "     INIT       "
      echo "****************"
      echo "install LCOV for code coverage"
      apt-get install -yq lcov
      echo "fetch nodejs"
      apt-get install -yq nodejs-legacy
      echo "fetch boost"
      apt-get install -yq  libboost-all-dev
      echo "fetch xsltproc"
      apt-get install -yq xsltproc
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
      echo "Clone from GitHub" 
      echo "****************"
      git clone https://review.gerrithub.io/serup/CloudChat
      cd CloudChat
      git checkout serup
      cd ..
      echo "CloudChat installed - from GitHub"
      echo "****************"
      echo "Set up swapfile"
      echo "****************"
      bash addswapfile.sh
      echo "****************"
      echo "  HADOOP SETUP"
      echo "****************"
      sudo apt-get install -yq rsync
      sudo apt-get install -yq ssh
      echo "****************"
      echo "hadoop_common"
      echo "****************"
      sudo wget -nc http://www.eu.apache.org/dist/hadoop/common/KEYS 2> /dev/null
      sudo wget -nc http://www.eu.apache.org/dist/hadoop/common/hadoop-2.7.1/hadoop-2.7.1.tar.gz.asc  2> /dev/null 
      sudo wget -nc http://www.eu.apache.org/dist/hadoop/common/hadoop-2.7.1/hadoop-2.7.1.tar.gz  2> /dev/null 
      sudo gpg --import KEYS  2> /dev/null  
      sudo gpg --verify hadoop-2.7.1.tar.gz.asc hadoop-2.7.1.tar.gz  2> /dev/null 
      sudo tar -xvf hadoop-2.7.1.tar.gz | nl | tail -1 | awk '{print "lines extracted:"  $1}'
      cd hadoop-2.7.1/bin
      ./hadoop version|grep Hadoop | awk '{print $2}' 
      cd ../..
      sudo mkdir input
      sudo cp ./hadoop-2.7.1/etc/hadoop/*.xml input
      ./hadoop-2.7.1/bin/hadoop jar ./hadoop-2.7.1/share/hadoop/mapreduce/hadoop-mapreduce-examples-2.7.1.jar grep input output 'dfs[a-z.]+'
      cat output/* 

      echo "****************"
      echo " taskwarrior "
      echo "****************"
      sudo apt-get install -yq taskwarrior

     fi #..
fi #.
