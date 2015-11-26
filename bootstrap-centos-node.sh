#!/bin/sh
# Run on VM to bootstrap Puppet Agent nodes

sudo rm -rf /var/lib/puppet/ssl/*

if ps aux | grep "puppet agent" | grep -v grep 2> /dev/null
then
    echo "Puppet Agent is already installed. Moving on..."
else
    sudo rpm -ivh http://yum.puppetlabs.com/puppetlabs-release-el-7.noarch.rpm
    sudo yum -y install puppet
fi

if cat /etc/puppet/puppet.conf | grep environment 2> /dev/null
then
    echo "Puppet Agent is already configured. Exiting..."
    sudo service puppet stop
    sudo rm -rf /var/lib/puppet/ssl/*
else
    #sudo apt-get update -yq && sudo apt-get upgrade -yq
    sudo rm -rf /var/lib/puppet/ssl/*

    # Configure /etc/hosts file
    echo "" >> /etc/hosts
    echo "# Host config for Puppet Master and Agent Nodes" >> /etc/hosts
    echo "192.168.31.8    dops.puppet.master  puppet" >> /etc/hosts
    echo "192.168.31.11   node01.dops.local  node01" >> /etc/hosts

    # Add agent section to /etc/puppet/puppet.conf
    echo "    server=puppet" >> /etc/puppet/puppet.conf
    echo "    environment=devtest" >> /etc/puppet/puppet.conf

    sudo cp /vagrant/nodes.rc.local /etc/rc.local
fi
