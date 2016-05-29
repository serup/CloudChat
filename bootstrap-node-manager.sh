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
      echo "*******************************" 
      echo " prerequisite for building"
      echo "*******************************" 
      echo "fetch nodejs"
      sudo apt-get install -yq nodejs-legacy
      echo "fetch boost"
      sudo apt-get install -yq  libboost-all-dev
      echo "fetch xsltproc"
      sudo apt-get install -yq xsltproc
      echo "fetch g++ since somehow gcc default install does not get it"
      sudo apt-get install -yq g++
      echo "install incron to monitor event/changes in transfer folder - new images added, should then remove old versions of same image"
      sudo apt-get install -yq incron
      sudo echo "root" >> /etc/incron.allow
      sudo echo "root" >> /etc/cron.allow
      sudo echo "vagrant" >> /etc/incron.allow
      sudo echo "vagrant" >> /etc/cron.allow
      sudo mkdir /var/www/img
      echo "*******************************" 
      echo "Clone from GitHub" 
      echo "*******************************" 
      git clone https://review.gerrithub.io/serup/CloudChat
      cd CloudChat
      git checkout serup
      echo "CloudChat installed"
      echo "*******************************" 
      echo " Swap file - needed for build "
      echo "*******************************" 
      echo "Set up swapfile"
      sudo bash addswapfile.sh
      echo "**************************" 
      echo "First time build CloudChat"
      echo "**************************" 
      echo "Building CloudChat project takes a long time - results are in file build.log - PLEASE WAIT!"
      sudo ./run.sh > build.log
      echo "done build - see info in file build.log"
      echo "Deploy to www"
      sudo mkdir -p /var/www/html/CloudChatManager/img
      sudo bash deploy_www.sh
      echo "**************************" 
      echo " cron job for cleaning "
      echo "**************************" 
      sudo chown vagrant:vagrant /var/www/html/CloudChatManager/img
      echo "copy cron job; clean old images to /usr/local/bin - the job is started by incron"
      sudo cp ./cleanoldimages.sh /usr/local/bin/cleanoldimages.sh
      sudo chmod +x /usr/local/bin/cleanoldimages.sh
      echo "setup incron job"
      incrontab -l | { cat; echo '/var/www/img IN_ALL_EVENTS /usr/local/bin/cleanoldimages.sh *.jpg >> /var/log/cleanoldimages.log 2>&1'; } | incrontab -
      echo "- done setup - now REBOOT, to start cron"
     fi
fi
