#!/usr/bin/env bash
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
cd mesos-1.1.0/build; sudo make install
sudo cp ~/GitHub/mesos-1.1.0/build/3rdparty/libprocess/.libs/libprocess.a /usr/lib/.
sudo cp ~/GitHub/mesos-1.1.0/build/3rdparty/libprocess/.libs/libprocess.la /usr/lib/. 
echo "- Done."
