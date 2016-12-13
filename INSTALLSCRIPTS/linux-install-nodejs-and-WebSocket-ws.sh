#!/usr/bin/env bash
echo "Install node "
mkdir -p /usr/local/src
cd /usr/local/src; sudo wget http://nodejs.org/dist/v0.8.21/node-v0.8.21.tar.gz
cd /usr/local/src; sudo tar -xvzf node-v0.8.21.tar.gz 
cd /usr/local/src/node-v0.8.21; sudo ./configure
cd /usr/local/src/node-v0.8.21; sudo make
cd /usr/local/src/node-v0.8.21; sudo make install
which node

echo "Install WebSocket (ws)"
