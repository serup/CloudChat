#!/bin/bash

echo "Install libboost - https://www.osetc.com/en/how-to-install-boost-on-ubuntu-16-04-18-04-linux.html"

sudo apt install libboost-dev
sudo apt install libboost-all-dev

dpkg -s libboost-dev | grep Version

