#!/bin/sh

# Run on VM to bootstrap Puppet Agent nodes
sudo rm -rf /var/lib/puppet/ssl/*

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

    # Setup java spring boot environment
#    sudo apt-get install -yq unzip
#    touch setup.sh
#    chmod 777 setup.sh
#    sudo chown vagrant:vagrant setup.sh
#    echo "su - vagrant" > setup.sh
#    curl -s get.sdkman.io >> setup.sh 
#    echo "source \"/root/.sdkman/bin/sdkman-init.sh\"" >> setup.sh
#    #echo "source \"/home/vagrant/.sdkman/bin/sdkman-init.sh\"" >> setup.sh
#    echo "echo installing grails" >> setup.sh
#    echo "sdk install grails 2>/dev/null " >> setup.sh
#    #echo "sudo ln -s /home/vagrant/.sdkman/candidates/grails/current/bin/grails /usr/bin/grails" >> setup.sh
#    echo "sudo ln -s /root/.sdkman/candidates/grails/current/bin/grails /usr/bin/grails" >> setup.sh
#    echo "sudo grails -version" >> setup.sh
#    bash setup.sh

    # Add agent section to /etc/puppet/puppet.conf
    echo "" | sudo tee --append /etc/puppet/puppet.conf 2> /dev/null && \
    echo "[agent]" | sudo tee --append /etc/puppet/puppet.conf 2> /dev/null && \
    echo "    server=puppet" | sudo tee --append /etc/puppet/puppet.conf 2> /dev/null && \
    echo "    environment=devtest" | sudo tee --append /etc/puppet/puppet.conf 2> /dev/null

    sudo cp /vagrant/nodes.rc.local /etc/rc.local
    sudo puppet agent --enable
fi
