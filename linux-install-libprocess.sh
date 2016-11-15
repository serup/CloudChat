#!/usr/bin/env bash
echo "Clone libprocess library "
cd ~/GitHub; git clone https://github.com/3rdparty/libprocess.git
echo "- install into /usr/include/process"
sudo cp -r ~/GitHub/libprocess/include/process/ /usr/include/
echo "- Done."
