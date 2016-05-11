#!/usr/bin/env bash -l
# Specify version here.  -- NB! MUST RUN as administrator -- sudo bash ./linux-install-codeblocks-sdk.sh
CODEBLOCKS_PACKAGE=codeblocks_16.01.tar.gz 
echo "NB! MUST RUN as administrator -- e.g: sudo bash ./linux-install-codeblocks-sdk.sh "
sudo apt-get install -yq libgtk2.0-dev build-essential checkinstall
sudo apt-get install -yq autoconf
sudo apt-get install -yq subversion

## if issues with gtk-3 - then remove it
## dpkg -r gtk

# using svn trunk
#svn checkout svn://svn.code.sf.net/p/codeblocks/code/trunk codeblocks-code
svn checkout http://svn.code.sf.net/p/codeblocks/code/trunk codeblocks-code
cd codeblocks-code
./update_revision.sh
./bootstrap
./configure 
sudo make
sudo make install

# Add the following line to the file /etc/ld.so.conf:

/usr/local/lib

...and run:

ldconfig

