#!/usr/bin/env bash -l
# Specify version here.  -- NB! MUST RUN as administrator -- sudo bash ./linux-install-codeblocks-sdk.sh
CODEBLOCKS_PACKAGE=codeblocks_16.01.tar.gz 
echo "NB! MUST RUN as administrator -- e.g: sudo bash ./linux-install-codeblocks-sdk.sh "
sudo apt-get install -yq libgtk2.0-dev build-essential checkinstall
sudo apt-get install -yq autoconf
sudo apt-get install -yq subversion

wget -O "/tmp/$CODEBLOCKS_PACKAGE" http://sourceforge.net/projects/codeblocks/files/Sources/16.01/$CODEBLOCKS_PACKAGE
cd /tmp

if [ -f "/tmp/$CODEBLOCKS_PACKAGE" ]; then
  tar -xjvf "$CODEBLOCKS_PACKAGE" 

  CODEBLOCKS_EXTRACTED_FOLDER=`ls -dt codeblocks-16* | head -1`
  sudo rsync -av "$CODEBLOCKS_EXTRACTED_FOLDER" /opt/codeblocks

fi

cd $CODEBLOCKS_EXTRACTED_FOLDER

# if issues with gtk-3 - then remove it
# dpkg -r gtk


./bootstrap
autoreconf -vfi
#./configure
#automake
#make
#sudo make install
./configure 
# set version to 16.01 -- important!!!
#sudo checkinstall
sudo make
sudo make install
