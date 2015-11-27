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

    # Add agent section to /etc/puppet/puppet.conf
    echo "" | sudo tee --append /etc/puppet/puppet.conf 2> /dev/null && \
    echo "[agent]" | sudo tee --append /etc/puppet/puppet.conf 2> /dev/null && \
    echo "    server=puppet" | sudo tee --append /etc/puppet/puppet.conf 2> /dev/null && \
    echo "    environment=devtest" | sudo tee --append /etc/puppet/puppet.conf 2> /dev/null

    sudo cp /vagrant/nodes.rc.local /etc/rc.local
    sudo puppet agent --enable

    echo "echo \"---------\" " >> .bashrc
    echo "echo \"\" " >> .bashrc
    echo "sudo grails --version" >> .bashrc
    echo "echo -n \"| \"; sudo spring version" >> .bashrc
    echo "echo -n \"| \"; mvn -version|grep --color=never 'Apache Maven'" >> .bashrc
    echo "echo \"---------\" " >> .bashrc
    echo "echo \"\" " >> .bashrc

    # IntelliJ - is installed as class idea in puppet/manifests/site
    # however the X11 forwarding is pt. not working
    #mkdir -p /usr/local/idea
    #cd /usr/local/idea
    #wget -O /tmp/intellij.tar.gz http://download.jetbrains.com/idea/ideaIC-12.0.4.tar.gz 2> /dev/null 
    #tar xfz /tmp/intellij.tar.gz 
    ## TODO: setup idea to work with port forward if possible - or set gui true 
    ##cd idea-IC-123.169/bin 
    ##./idea.sh < /dev/null 
    #cd -

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
      echo "build javaspring project"
      cd javaspring_projects/gs-messaging-stomp-websocket/initial
      ./gradlew
      echo "********************************************************************************************************************"
      echo "start intellij idea editor - NB! inorder for x11 forward to work, then start like this: vagrant -XY ssh javaservices"
      echo "********************************************************************************************************************"
      cd /opt/idea-15.0.1/idea-IC-143.382.35/bin/
      sudo ./idea.sh
    fi

fi
