#!/usr/bin/env bash
# TODO: Find a way to install all dependencies - MESOS project should be looked at instead - somehow this project fails and mesos as libprocess inside
echo "Clone libprocess library "
cd ~/GitHub; git clone https://github.com/3rdparty/libprocess.git
echo "- install into /usr/include/stout"
cd ~/GitHub/libprocess; ./bootstrap
cd ~/GitHub/libprocess; ./configure
cd ~/GitHub/libprocess; sudo make 
echo "- build stout"
cd ~/GitHub/libprocess/3rdparty/stout; ./configure
echo "- install into /usr/include/stout/"
sudo cp -r ~/GitHub/libprocess/3rdparty/stout/include/stout/ /usr/include/
echo "- install into /usr/include/process"
sudo cp -r ~/GitHub/libprocess/include/process/ /usr/include/
echo "- Done."
