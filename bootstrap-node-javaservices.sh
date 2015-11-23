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
#    echo "" | sudo tee --append /etc/hosts 2> /dev/null && \
#    echo "# Host config for Puppet Master and Agent Nodes" | sudo tee --append /etc/hosts 2> /dev/null && \
#    echo "192.168.32.8    puppet.dops.local           puppet" | sudo tee --append /etc/hosts 2> /dev/null && \
#    echo "192.168.32.19   javaservices.scanva.com     javaservices" | sudo tee --append /etc/hosts 2> /dev/null && \
#    echo "192.168.32.21   backend.scanva.com          backend" | sudo tee --append /etc/hosts 2> /dev/null && \
#    echo "192.168.32.22   cloudchatmanager.com        cloudchatmanager" | sudo tee --append /etc/hosts 2> /dev/null && \
#    echo "192.168.32.23   cloudchatclient.com         cloudchatclient" | sudo tee --append /etc/hosts 2> /dev/null && \
#    echo "192.168.32.20   jenkins.scanva.com          jenkins" | sudo tee --append /etc/hosts 2> /dev/null
    sudo bash /vagrant/confighosts.sh

    # Setup java spring boot environment
#    echo "- install unzip"
#    sudo apt-get install -yq unzip 
#    echo "- install curl"
#    sudo apt-get install -yq curl 
#    echo "- install sdkman"
#    touch tmp.sh
#    sudo chmod 777 tmp.sh
#    curl -s http://get.sdkman.io > tmp.sh 
#    echo "echo - init sdkman " >> tmp.sh
#    echo "source ~/.sdkman/bin/sdkman-init.sh" >> tmp.sh 
#    echo "echo - init sdk" >> tmp.sh
#    #source .sdkman/bin/sdkman-init.sh
#    echo "source \"/home/vagrant/.sdkman/bin/sdkman-init.sh\"" >> tmp.sh
#    #echo "echo - test gvm" >> tmp.sh
#    #echo "gvm version" >> tmp.sh
#    #echo "echo - install grails" >> tmp.sh
#    sudo bash tmp.sh
#    gvm install grails
#    echo " - done."

    sudo apt-get install -yq unzip
    touch setup.sh
    chmod 777 setup.sh
    curl -s get.sdkman.io > setup.sh 
    echo "source \"/root/.sdkman/bin/sdkman-init.sh\"" >> setup.sh
    echo "echo installing grails" >> setup.sh
    echo "sdk install grails 2>/dev/null " >> setup.sh
    echo "sudo ln -s /home/vagrant/.sdkman/candidates/grails/current/bin/grails grails" >> setup.sh
    echo "sudo grails -version" >> setup.sh
    sudo chown vagrant setup.sh
    bash setup.sh

    # Add agent section to /etc/puppet/puppet.conf
    echo "" | sudo tee --append /etc/puppet/puppet.conf 2> /dev/null && \
    echo "[agent]" | sudo tee --append /etc/puppet/puppet.conf 2> /dev/null && \
    echo "    server=puppet" | sudo tee --append /etc/puppet/puppet.conf 2> /dev/null && \
    echo "    environment=devtest" | sudo tee --append /etc/puppet/puppet.conf 2> /dev/null

    sudo cp /vagrant/nodes.rc.local /etc/rc.local
    sudo puppet agent --enable
fi
