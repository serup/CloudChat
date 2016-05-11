#!/usr/bin/env bash -l
# http://www.binarytides.com/install-wxwidgets-ubuntu/
# Specify version here.  -- NB! MUST RUN as administrator -- sudo bash ./linux-install-wxwidget.sh
WXWIDGET_PACKAGE=wxWidgets-3.0.2.tar.bz2
echo "NB! MUST RUN as administrator -- e.g: sudo bash ./linux-install-wxwidget.sh "

DEB_PKG_OK=$(fgrep deb /etc/apt/sources.list  2>&1 | grep "codelite*")
if [ "" == "$DEB_PKG_OK" ]; then
  echo -n "- add codelite deb package on ubuntu, to use for codeblocks plugins development "
  sudo apt-add-repository 'deb http://repos.codelite.org/wx3.0.2/ubuntu/ trusty universe'
  sudo apt-get update
  echo " - done."
else
  echo "- codelite deb package already installed"
fi

sudo apt-get install libwxbase3.0-0-unofficial \
                 libwxbase3.0-dev \
                 libwxgtk3.0-0-unofficial \
                 libwxgtk3.0-dev \
                 wx3.0-headers \
                 wx-common \
                 libwxbase3.0-dbg \
                 libwxgtk3.0-dbg \
                 wx3.0-i18n \
                 wx3.0-examples \
                 wx3.0-doc

sudo apt-get install -yq libgtk-3-dev build-essential checkinstall

wget -O "/tmp/$WXWIDGET_PACKAGE" https://github.com/wxWidgets/wxWidgets/releases/download/v3.0.2/$WXWIDGET_PACKAGE
cd /tmp

if [ -f "/tmp/$WXWIDGET_PACKAGE" ]; then
  tar -xjvf "$WXWIDGET_PACKAGE" 

  WXWIDGET_EXTRACTED_FOLDER=`ls -dt wxWidget* | grep -v *.tar* | head -1`
  sudo rsync -av "$WXWIDGET_EXTRACTED_FOLDER" /opt/wxwidget

fi

cd $WXWIDGET_EXTRACTED_FOLDER

echo "Compile wxWidgets "
if [ -d "gtk-build" ]; then
  	echo "gtk-build already exists"
else
	mkdir gtk-build
fi
cd gtk-build/
../configure --disable-shared --enable-unicode
echo "Build wxWidget"
make
echo "Install wxWidget with checkinstall - you MUST change version number to 3.0 - otherwise install will fail!!!"
sudo checkinstall

# 
# **********************************************************************
#
# Done. The new package has been installed and saved to
#
# /tmp/wxWidgets-3.0.2/gtk-build/gtk_3.0-1_amd64.deb
#
# You can remove it from your system anytime using: 
#
#      dpkg -r gtk
#
#**********************************************************************

echo "Compile samples"
cd samples
make

