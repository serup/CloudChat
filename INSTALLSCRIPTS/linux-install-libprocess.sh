#!/usr/bin/env bash
# TODO: Find a way to install all dependencies - MESOS project should be looked at instead - somehow this project fails and mesos as libprocess inside
# echo "Clone libprocess library "
# cd ~/GitHub; git clone https://github.com/3rdparty/libprocess.git
# echo "- install into /usr/include/stout"
# cd ~/GitHub/libprocess; ./bootstrap
# cd ~/GitHub/libprocess; mkdir build 
# cd ~/GitHub/libprocess/build; ../configure
# cd ~/GitHub/libprocess/build; make 
# cd ~/GitHub/libprocess/build; make check
# cd ~/GitHub/libprocess/build; sudo make install 
# 
# #echo "- build stout"
# #cd ~/GitHub/libprocess/3rdparty/stout; ./configure
# #echo "- install into /usr/include/stout/"
# #sudo cp -r ~/GitHub/libprocess/3rdparty/stout/include/stout/ /usr/include/
# #echo "- install into /usr/include/process"
# #sudo cp -r ~/GitHub/libprocess/include/process/ /usr/include/

# Update the packages.
sudo apt-get update

# Install a few utility tools.
sudo apt-get install -y tar wget git

# Install the latest OpenJDK.
sudo apt-get install -y openjdk-8-jdk

# Install autotools (Only necessary if building from git repository).
sudo apt-get install -y autoconf libtool

# Install other Mesos dependencies.
sudo apt-get -y install build-essential python-dev libcurl4-nss-dev libsasl2-dev libsasl2-modules maven libapr1-dev libsvn-dev zlib1g-dev

echo "Clone mesos and take libprocess"
cd ~/GitHub; wget http://www.apache.org/dist/mesos/1.1.0/mesos-1.1.0.tar.gz
cd ~/GitHub; tar -zxf mesos-1.1.0.tar.gz
echo "Build mesos and take libprocess.a "
cd ~/GitHub/mesos-1.1.0; ./bootstrap
cd ~/GitHub/mesos-1.1.0; mkdir -p build;
cd ~/GitHub/mesos-1.1.0/build; ../configure
cd ~/GitHub/mesos-1.1.0/build; make
cd ~/GitHub/mesos-1.1.0/build; make check
#cd mesos-1.1.0/build; sudo make install
sudo cp ~/GitHub/mesos-1.1.0/build/3rdparty/libprocess/.libs/libprocess.a /usr/lib/.
sudo cp ~/GitHub/mesos-1.1.0/build/3rdparty/libprocess/.libs/libprocess.la /usr/lib/. 
echo "- Done."
