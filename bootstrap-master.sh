#!/bin/sh

# Run on VM to bootstrap Puppet Master server

if ps aux | grep "puppet master" | grep -v grep 2> /dev/null
then
    echo "Puppet Master is already installed. Exiting..."
    sudo cp /vagrant/puppet-master.conf /etc/puppet/puppet.conf
    sudo cp /vagrant/autosign.conf /etc/puppet/autosign.conf
    sudo puppet cert clean node*

else
    # Install Puppet Master
    sudo cp /vagrant/sources.list /etc/apt/sources.list
    #wget https://apt.puppetlabs.com/puppetlabs-release-trusty.deb && \
    #sudo dpkg -i puppetlabs-release-trusty.deb && \
    wget https://apt.puppetlabs.com/puppetlabs-release-pc1-vivid.deb && \
    sudo dpkg -i puppetlabs-release-pc1-vivid.deb && \
    sudo apt-get update -yq && sudo apt-get upgrade -yq

    sudo rm -f /var/run/puppet/master.pid
    sudo apt-get install -yq puppetmaster

    # Configure /etc/hosts file
    sudo cp /vagrant/confighosts.sh .
    sudo bash confighosts.sh

    # Add optional alternate DNS names to /etc/puppet/puppet.conf
    sudo cp /vagrant/puppet-master.conf /etc/puppet/puppet.conf
    sudo cp /vagrant/autosign.conf /etc/puppet/autosign.conf

    # symlink manifest from Vagrant synced folder location
    sudo rm -fr /etc/puppet/manifests
    ln -s /vagrant/manifests /etc/puppet/manifests

    sudo service puppet stop
    sudo service puppetmaster stop
    sudo rm -rf /var/lib/puppet/ssl/*
    sudo service puppetmaster start
    sudo service puppet start
    sudo cp /vagrant/master.rc.local /etc/rc.local
fi
