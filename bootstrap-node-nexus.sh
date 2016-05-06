#!/bin/sh

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

    echo "fetch nodejs"
    sudo apt-get install -yq nodejs-legacy

    echo "fetch boost"
    sudo apt-get install -yq  libboost-all-dev
    
    echo "fetch xsltproc"
    sudo apt-get install -yq xsltproc

    echo "fetch g++ since somehow gcc default install does not get it"
    sudo apt-get install -yq g++


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

fi
