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
    echo "  JEKYLL SETUP"
    echo "****************"
    sudo jekyll new jekyll_info_site
    cd jekyll_info_site
    sudo jekyll build -d /var/www/html --incremental
    echo "/usr/local/bin/jekyll build --watch -s /home/vagrant/jekyll_info_site -d /var/www/" | sudo tee --append /etc/rc.local 2> /dev/null 

    echo "**********************************************************************************************************************"

    sudo ssh-keygen -t dsa -P '' -f ~/.ssh/id_dsa
    sudo cat ~/.ssh/id_dsa.pub >> ~/.ssh/authorized_keys

    echo "*******************************"


