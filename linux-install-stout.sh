#!/usr/bin/env bash
echo "Clone stout library "
#cd ~GitHub; git clone https://github.com/3rdparty/stout.git
cd ~/GitHub; git clone https://github.com/apache/mesos.git
echo "- prereq - needed to build"
# Install autotools (Only necessary if building from git repository).
sudo apt-get install -y autoconf libtool

# Install other Mesos dependencies.
sudo apt-get -y install build-essential python-dev libcurl4-nss-dev libsasl2-dev libsasl2-modules maven libapr1-dev libsvn-dev zlib1g-dev

echo "- build mesos"
cd ~/GitHub/mesos; ./bootstrap
cd ~/GitHub/mesos; ./configure
cd ~/GitHub/mesos; make

echo "- install into /usr/include/stout"
sudo cp -r ~/GitHub/mesos/3rdparty/stout/include/stout/ /usr/include/
echo "- Done."
