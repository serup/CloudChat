#!/usr/bin/env bash
PKG_OK=$(dpkg-query -W --showformat='${Status}\n' 2>&1 *zookeeper-server* |grep "install ok installed")
if [ "" == "$PKG_OK" ]; then
	echo -n "- install zookeeper "
	# https://www.tutorialspoint.com/zookeeper/zookeeper_installation.htm
	wget http://mirrors.rackhosting.com/apache/zookeeper/stable/zookeeper-3.4.9.tar.gz 
	sudo tar -zxf zookeeper-3.4.9.tar.gz -C /opt/
	sudo mkdir /opt/zookeeper-3.4.9/data
	sudo cp /opt/zookeeper-3.4.9/conf/zoo_sample.cfg /opt/zookeeper-3.4.9/conf/zoo.cfg
	sudo sed -i '/#/d' /opt/zookeeper-3.4.9/conf/zoo.cfg
	echo " - done."
else
	echo "- zookeeper already installed"
fi
