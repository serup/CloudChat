#!/usr/bin/env bash
echo "Clone mesos library "
#cd ~/GitHub; git clone https://github.com/apache/mesos.git
cd ~/GitHub; git clone https://git-wip-us.apache.org/repos/asf/mesos.git

echo "- prereq - needed to build"
# Install autotools (Only necessary if building from git repository).
sudo apt-get install -y autoconf libtool

# Install other Mesos dependencies.
sudo apt-get -y install build-essential python-dev libcurl4-nss-dev libsasl2-dev libsasl2-modules maven libapr1-dev libsvn-dev zlib1g-dev

echo "- build mesos"
cd ~/GitHub/mesos; ./bootstrap
mkdir -p ~/GitHub/mesos/build
cd ~/GitHub/mesos/build; ../configure
cd ~/GitHub/mesos/build; make

#echo "- install into /usr/include/stout - do NOT overwrite"
#sudo cp -ir ~/GitHub/mesos/3rdparty/stout/include/stout/ /usr/include/
#echo "- copy static library libprocess to default /usr/lib"
#sudo cp -ir ~/GitHub/mesos/3rdparty/libprocess/.libs/libprocess.a /usr/lib/
echo "check build"
cd ~/GitHub/mesos/build; make check
echo "install mesos"
cd ~/GitHub/mesos/build; make install 
echo "- Done."
