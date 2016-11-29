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
    echo "echo -n \"| \"; ls /opt/ | grep --color=never 'idea-'" >> .bashrc
    echo "echo \"---------\" " >> .bashrc
    echo "echo \"\" " >> .bashrc

    # IntelliJ - is installed as class idea in puppet/manifests/site

    echo "fetch nodejs"
    sudo apt-get install -yq nodejs-legacy
    echo "fetch boost"
    sudo apt-get install -yq  libboost-all-dev
    echo "fetch xsltproc"
    sudo apt-get install -yq xsltproc
    echo "fetch g++ since somehow gcc default install does not get it"
    sudo apt-get install -yq g++

    echo "install libraries so X11 forwarding will work for javaFX applications"
    sudo apt-get install libgtk2.0-0 libgdk-pixbuf2.0-0 libfontconfig1 libxrender1 libx11-6 libglib2.0-0  libxft2 libfreetype6 libc6 zlib1g libpng12-0 libstdc++6-4.8-dbg-arm64-cross libgcc1 
    sudo apt-get install libcanberra-gtk3-module

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
      echo "copy commit-msg - to enable git push from this entity"
      curl -Lo .git/hooks/commit-msg http://review.gerrithub.io/tools/hooks/commit-msg
#incase above fails - use old version of commit-msg script      
#cp commit-msg .git/hooks/.
      chmod u+x .git/hooks/commit-msg
      echo "Set up swapfile"
      sudo bash addswapfile.sh
      echo "build javaspring project"
      cd javaspring_projects/gs-messaging-stomp-websocket/initial
      ./gradlew
      echo "********************************************************************************************************************"
      echo "use intellij idea editor - NB! inorder for x11 forward to work, then start like this: vagrant -XY ssh javaservices"
      echo " start idea when loged in: /usr/bin/idea or sudo idea"
      echo "********************************************************************************************************************"
      sudo ln -s /opt/idea-2016.2/idea-IC-162.1121.32/bin/idea.sh /usr/bin/idea
    fi

fi
