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
      sudo apt-get install -yq lcov
      echo "fetch nodejs"
      sudo apt-get install -yq nodejs-legacy
      echo "fetch boost"
      sudo apt-get install -yq  libboost-all-dev
      echo "fetch xsltproc"
      sudo apt-get install -yq xsltproc
      echo "fetch g++ since somehow gcc default install does not get it"
      sudo apt-get install -yq g++
      echo "install sshpass to allow automatic transfer of images script to run - consider using ssh keys instead in future"
      sudo apt-get install -yq sshpass
      echo "install incron to monitor event/changes in transfer folder - new images added, should then start transfer"
      sudo apt-get install -yq incron
      sudo echo "root" >> /etc/incron.allow
      sudo echo "root" >> /etc/cron.allow
      sudo echo "vagrant" >> /etc/incron.allow
      sudo echo "vagrant" >> /etc/cron.allow
      sudo mkdir /var/www/img
      echo "****************"
      echo "Clone from GitHub" 
      echo "****************"
      git clone https://review.gerrithub.io/serup/CloudChat
      cd CloudChat
      git checkout serup
      echo "CloudChat installed - from GitHub"
      echo "****************"
      echo "Set up swapfile"
      echo "****************"
      sudo bash addswapfile.sh
      echo "****************"
      echo "First time build"
      echo "****************"
      echo "Building CloudChat project takes a long time - results are in file build.log - PLEASE WAIT!"
      sudo ./run.sh > build.log
      echo "done build - see info in file build.log"
      echo "create lcov code coverage info "
      sudo lcov --capture --directory . --output-file coverage.info
      echo "generate html code coverage info in /www/lcov"
      sudo genhtml coverage.info --output-directory /var/www/lcov > /dev/null
      echo "****************"
      echo "setup incron job"
      echo "****************"
      echo "copy cron replication job to /usr/local/bin - the job is started by incron, and it copies from backend to cloudchatmanager"
      sudo cp ./replication.sh /usr/local/bin/.
      sudo chown vagrant:vagrant /usr/local/bin/replication.sh
      sudo chmod +x /usr/local/bin/replication.sh
      sudo cp ./cronStartServer.sh /usr/local/bin/.
      sudo chown vagrant:vagrant /usr/local/bin/cronStartServer.sh
      sudo chmod +x /usr/local/bin/cronStartServer.sh
      echo "replication deamon - should copy files using sshpass scp - its setup as a cron job"
      incrontab -l | { cat; echo '/var/www/img IN_ALL_EVENTS /usr/local/bin/replication.sh >> /var/log/replication.log 2>&1'; } | incrontab -
      echo "****************"
      echo "start backend server - as a crontab job"
      echo "****************"
      sudo -s
      crontab -l | { cat; echo '@reboot /usr/local/bin/cronStartServer.sh >> /var/log/crontab.log 2>&1'; } | crontab -
      echo "- done setup - now REBOOT, to start cron"
      reboot
     fi
fi
