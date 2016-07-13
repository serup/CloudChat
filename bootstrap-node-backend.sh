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
      echo "root" >> /etc/incron.allow
      echo "root" >> /etc/cron.allow
      echo "vagrant" >> /etc/incron.allow
      echo "vagrant" >> /etc/cron.allow
      mkdir /var/www/img
      echo "****************"
      echo "Clone from GitHub" 
      echo "****************"
      git clone https://review.gerrithub.io/serup/CloudChat
      cd CloudChat
      git checkout serup
      git pull
      echo "CloudChat installed - from GitHub"
      echo "****************"
      echo "Set up swapfile"
      echo "****************"
      bash addswapfile.sh
      # http://www.tweaking4all.com/software/linux-software/use-xrdp-remote-access-ubuntu-14-04/ 
      # NB! use Remmina - remote desktop client with RDP, on your Ubuntu to connect to this node
      echo "***********************************************"
      echo "Set up Remote desktop handling - XRDP and XFCE" 
      echo "***********************************************"
      sudo apt-get update
      sudo apt-get install -yq xrdp
      sudo apt-get install -yq xfce4
      # install XFCE4 terminal (way better than xterm)
      sudo apt-get install -yq xfce4-terminal
      # Install icon sets
      sudo apt-get install -yq gnome-icon-theme-full tango-icon-theme
      echo xfce4-session >~/.xsession
      sudo sed -i 's/. \/etc\/X11\/Xsession/startxfce4/' /etc/xrdp/startwm.sh
      sudo service xrdp restart
      echo "*** use second ip to connect via Remmina with RDP, or backend.scanva.com"
      hostname -I
      echo "- end setup of remote desktop handling"
      echo "****************"
      echo "First time build"
      echo "****************"
      #alias make="/usr/bin/make -j 8"  --- NB! do NOT use -j switch !! -  it somehow create bad build - Perhaps makefiles are not up to running parallel
      echo "Building CloudChat project takes a long time - results are in file build.log - PLEASE WAIT!"
      ./run.sh > build.log 2> tmp3
      echo "done build - see detailed info in file build.log"
      tail -5 build.log
      echo "************************************"
      echo "create lcov code coverage info "
      echo "************************************"
      lcov --capture --directory . --output-file coverage.info
      echo "generate html code coverage info in /www/lcov"
      genhtml coverage.info --output-directory /var/www/lcov 2> /dev/null
      echo "****************"
      echo "setup incron job"
      echo "****************"
      echo "copy cron replication job to /usr/local/bin - the job is started by incron, and it copies from backend to cloudchatmanager"
      cp ./replication.sh /usr/local/bin/.
      chown root:root /usr/local/bin/replication.sh
      chmod +x /usr/local/bin/replication.sh
      cp ./cronStartServer.sh /usr/local/bin/.
      chown root:root /usr/local/bin/cronStartServer.sh
      chmod +x /usr/local/bin/cronStartServer.sh
      echo "replication deamon - should copy files using sshpass scp - its setup as a cron job"
      incrontab -l | { cat; echo '/var/www/img IN_CREATE /usr/local/bin/replication.sh >> /var/log/replication.log 2>&1 $@/$#'; } | incrontab -
      echo "****************"
      echo "start backend server - as a crontab job"
      echo "****************"
      crontab -l | { cat; echo '@reboot /usr/local/bin/cronStartServer.sh >> /var/log/crontab.log 2>&1'; } | crontab -
      echo "- done setup - now REBOOT, to start cron"
      reboot
     fi
fi
