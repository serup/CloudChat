#!/usr/bin/env bash
echo "*******************************************************************************"
echo "Install Facebook folly lib - library for handling Promises and Futures in C++11"
echo "- info here: https://github.com/facebook/folly"
echo "*******************************************************************************"
echo "Fetch dependencies"
#sudo apt-get install \
#	g++ \
#	automake \
#	autoconf \
#	autoconf-archive \
#	libtool \
#	libboost-all-dev \
#	libevent-dev \
#	libdouble-conversion-dev \
#	libgoogle-glog-dev \
#	libgflags-dev \
#	liblz4-dev \
#	liblzma-dev \
#	libsnappy-dev \
#	make \
#	zlib1g-dev \
#	binutils-dev \
#	libjemalloc-dev \
#	libssl-dev \
#	libiberty-dev

echo "*******************************************************************************"
echo "Fetch folly from GitHub"
mkdir -p ~/GitHub
cd ~/GitHub; git clone https://github.com/facebook/folly.git 
echo "configure and install library"
cd ~/GitHub/folly/folly; autoreconf -ivf
cd ~/GitHub/folly/folly; ./configure
cd ~/GitHub/folly/folly; make
cd ~/GitHub/folly/folly; make check
cd ~/GitHub/folly/folly; sudo make install 
echo "Done."
