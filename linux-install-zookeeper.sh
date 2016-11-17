#!/usr/bin/env bash
PKG_OK=$(dpkg-query -W --showformat='${Status}\n' 2>&1 zookeeper |grep "install ok installed")
if [ "" == "$PKG_OK" ]; then
	echo -n "- install zookeeper "
	# https://www.tutorialspoint.com/zookeeper/zookeeper_installation.htm
	wget http://mirrors.rackhosting.com/apache/zookeeper/zookeeper-3.4.8/zookeeper-3.4.8.tar.gz
	sudo tar -zxf zookeeper-3.4.8.tar.gz -C /opt/
	sudo mkdir /opt/zookeeper-3.4.8/data
	sudo cp /opt/zookeeper-3.4.8/conf/zoo_sample.cfg /opt/zookeeper-3.4.8/conf/zoo.cfg
	sudo sed -i '/#/d' /opt/zookeeper-3.4.8/conf/zoo.cfg
	#wget http://mirrors.rackhosting.com/apache/zookeeper/stable/zookeeper-3.4.9.tar.gz 
	#sudo tar -zxf zookeeper-3.4.9.tar.gz -C /opt/
	#sudo mkdir /opt/zookeeper-3.4.9/data
	#sudo cp /opt/zookeeper-3.4.9/conf/zoo_sample.cfg /opt/zookeeper-3.4.9/conf/zoo.cfg
	sudo sed -i '/#/d' /opt/zookeeper-3.4.9/conf/zoo.cfg
	echo " - done."
else
	echo "- zookeeper already installed"
fi

PKG_OK=$(dpkg-query -W --showformat='${Status}\n' libev-dev |grep "install ok installed")
if [ "" == "$PKG_OK" ]; then
	echo -n "- install libcheck "
	sudo apt-get install -yq libev-dev 
	echo " - done."
else
	echo "- libev already installed"
fi

PKG_OK=$(dpkg-query -W --showformat='${Status}\n' libzookeeper-mt-dev |grep "install ok installed")
if [ "" == "$PKG_OK" ]; then
	echo -n "- install libzookeeper-mt-dev "
	sudo apt-get install -yq libzookeeper-mt-dev
	echo " - done."
else
	echo "- libzookeeper-mt-dev already installed"
fi

PKG_OK=$(dpkg-query -W --showformat='${Status}\n' libgoogle-glog-dev |grep "install ok installed")
if [ "" == "$PKG_OK" ]; then
	echo -n "- install libgoogle-glog-dev "
	sudo apt-get install -yq libgoogle-glog-dev
	echo " - done."
else
	echo "- libgoogle-glog-dev already installed"
fi


