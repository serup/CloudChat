#!/usr/bin/env bash

echo "This will fetch prerequisites for protobuf project, then clone protobuf project and build it to install it"
sudo apt-get install autoconf automake libtool curl make g++ unzip
mkdir -p ~/GitHub/google
cd ~/GitHub/google; git clone https://github.com/google/protobuf.git
cd ~/GitHub/google/protobuf; ./autogen.sh
cd ~/GitHub/google/protobuf; ./configure
cd ~/GitHub/google/protobuf; make
cd ~/GitHub/google/protobuf; make check
cd ~/GitHub/google/protobuf; sudo make install
cd ~/GitHub/google/protobuf; sudo ldconfig # refresh shared library cache.
