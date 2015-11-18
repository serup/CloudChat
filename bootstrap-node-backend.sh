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
    echo "" | sudo tee --append /etc/hosts 2> /dev/null && \
    echo "# Host config for Puppet Master and Agent Nodes" | sudo tee --append /etc/hosts 2> /dev/null && \
    echo "192.168.32.8    puppet.dops.local      puppet" | sudo tee --append /etc/hosts 2> /dev/null && \
    echo "192.168.32.21   backend.scanva.com     backend" | sudo tee --append /etc/hosts 2> /dev/null && \
    echo "192.168.32.22   cloudchatmanager.com   cloudchatmanager" | sudo tee --append /etc/hosts 2> /dev/null && \
    echo "192.168.32.23   cloudchatclient.com    cloudchatclient" | sudo tee --append /etc/hosts 2> /dev/null && \
    echo "192.168.32.20   jenkins.scanva.com     jenkins" | sudo tee --append /etc/hosts 2> /dev/null


    # Add agent section to /etc/puppet/puppet.conf
    echo "" | sudo tee --append /etc/puppet/puppet.conf 2> /dev/null && \
    echo "[agent]" | sudo tee --append /etc/puppet/puppet.conf 2> /dev/null && \
    echo "    server=puppet" | sudo tee --append /etc/puppet/puppet.conf 2> /dev/null && \
    echo "    environment=devtest" | sudo tee --append /etc/puppet/puppet.conf 2> /dev/null

    sudo cp /vagrant/nodes.rc.local /etc/rc.local

    # bugfix hostname for ubuntu 15.04 - run this command    
    sudo bash /etc/init.d/hostname.sh


    sudo puppet agent --enable

    echo "fetch nodejs"
    sudo apt-get install -yq nodejs-legacy

    echo "fetch boost"
    sudo apt-get install -yq  libboost-all-dev
    
    echo "fetch xsltproc"
    sudo apt-get install -yq xsltproc

    echo "fetch g++ since somehow gcc default install does not get it"
    sudo apt-get install -yq g++

    echo "Fetch latest version of CloudChat"
    if [ -d "CloudChat" ]; then
      echo "CloudChat already installed"
      echo "updating ..."
      cd CloudChat
      git checkout serup
      git pull
    else
      git clone https://review.gerrithub.io/serup/CloudChat
      cd CloudChat
      git checkout serup
      echo "CloudChat installed"
      echo "Set up swapfile"
      sudo bash addswapfile.sh
      echo "Build CloudChat"
      sudo ./run.sh > build.log
      echo "done build - see info in file build.log"
      sudo -s
      cd codeblocks_projects
      echo "start backend server"
      ./startScanvaserver
     fi
fi
