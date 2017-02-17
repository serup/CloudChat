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

	echo "********************************"
	echo "install Apache NiFi"
	echo " takes long time be patient...."
	echo "********************************"
	mkdir -p NiFi
	cd NiFi
	wget http://ftp.download-by.net/apache/nifi/1.1.1/nifi-1.1.1-bin.tar.gz . 2> download.log
	wget https://www.apache.org/dist/nifi/1.1.1/nifi-1.1.1-bin.tar.gz.asc . 2> download_asc.log
	echo "import keys for verification of download"
    gpg --import  KEYS
	gpg --keyserver pgpkeys.mit.edu --recv-key 22886FEE
	gpg --verify nifi-1.1.1-bin.tar.gz.asc nifi-1.1.1-bin.tar.gz
	gpg --fingerprint 22886FEE
	echo "****************************************************************"
	echo " please verify that Good signature is received"
	echo " more info here: https://www.apache.org/info/verification.html"
	echo "****************************************************************"
	echo " Now extract"
	tar -xvf nifi-1.1.1-bin.tar.gz > extract.log
	echo "****************"
	echo "Done - installation of nifi"
	echo "**********************************"
	echo " install NiFi as service "
	echo "**********************************"
	sudo /home/vagrant/NiFi/nifi-1.1.1/bin/nifi.sh install 
	sudo service nifi status 
	#sudo /home/vagrant/NiFi/nifi-1.1.1/bin/nifi.sh start
	
	echo "**********************************"
	echo " example template "
	echo "**********************************"
	echo " try testfile: (to test run then copy it to folder tranferToNiFi; it should be originally stored in files folder) "
	echo " #"
	echo " #"
	echo " #"
	echo " #"
	echo " #"
	echo " #"
	echo " # bla bla"
	echo " #"
	echo " #"
	echo " 0.0.0.0|0.255.255.255|0|0|20108000|0|001|0|XX|0|||||2|123|HEE=Text line 1 bla bla"
	echo " 1.0.0.0|1.0.0.0|0|0|20108000|0|001|0|XX|0|||||2|123|HEE=Text line 2 bla bla"
	echo " 1.0.0.1|1.0.0.1|2.72|101.31|40000|0|001|0|XX|0|||||2|123|HEE=Text line 3 bla bla"
	sudo mkdir -p /home/vagrant/NiFi/nifi-1.1.1/templates
	sudo chown vagrant /home/vagrant/NiFi/nifi-1.1.1/templates
	cp /vagrant/nifiexampletemplate.xml /home/vagrant/NiFi/nifi-1.1.1/templates/.
	sudo mkdir -p /home/vagrant/NiFi/nifi-1.1.1/work
	sudo chown vagrant /home/vagrant/NiFi/nifi-1.1.1/work 
	mkdir -p /home/vagrant/NiFi/nifi-1.1.1/work/test/files
	mkdir -p /home/vagrant/NiFi/nifi-1.1.1/work/test/result
	mkdir -p /home/vagrant/NiFi/nifi-1.1.1/work/test/transferToNiFi
fi


echo "****************"
echo "start nifi"
echo "****************"
sudo /home/vagrant/NiFi/nifi-1.1.1/bin/nifi.sh start 
#sudo service nifi start
echo "****************"
echo "You can see nifi here : http://nifi.dops.scanva.com:8080/nifi or http://192.168.31.12:8080/nifi/"

