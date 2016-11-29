#!/usr/bin/env bash
echo "Clone picojson library "
cd ~/GitHub; git clone https://github.com/kazuho/picojson.git
echo "- install into /usr/include/stout/"
cd ~/GitHub/picojson; sudo make install
echo "- Done."

