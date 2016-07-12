#!/bin/sh
# Specify IDE version here.  -- NB! MUST RUN as administrator - like this: sudo bash  ./linux-install-VSCODE-ide.sh
# if this fails - then try following to install version 1.3
#   first download -  https://go.microsoft.com/fwlink/?LinkID=760868
#   then install   -  sudo dpkg -i code_1.3.0-1467909982_amd64.deb

PLATFORM=$(uname -i)
if [ "i686" == "$PLATFORM" ]; then
  VSCODE_PACKAGE=VSCode-linux-ia32-stable.zip
else
  VSCODE_PACKAGE=VSCode-linux-x64-stable.zip
fi

echo -n " - do you want to remove old version (y/n)? "
read answer
if echo "$answer" | grep -iq "^y" ;then
   echo "- destroy old setup, so new can run"
   sudo rm -rf /opt/vscode
   sudo mkdir /opt/vscode
   sudo rm -rf /tmp/VSCode*
else
   echo "- old setup will NOT be destroyed"
fi

if [ -f "/tmp/$VSCODE_PACKAGE" ]; then
  echo "package exists already - no need to download"
else
  if [ "i686" == "$PLATFORM" ]; then
     wget -O "/tmp/$VSCODE_PACKAGE" https://az764295.vo.msecnd.net/stable/def9e32467ad6e4f48787d38caf190acbfee5880/$VSCODE_PACKAGE
  else
     wget -O "/tmp/$VSCODE_PACKAGE" https://az764295.vo.msecnd.net/stable/e724f269ded347b49fcf1657fc576399354e6703/$VSCODE_PACKAGE
    #wget -O "/tmp/$VSCODE_PACKAGE" https://az764295.vo.msecnd.net/stable/e724f269ded347b49fcf1657fc576399354e6703/$VSCODE_PACKAGE
    #wget -O "/tmp/$VSCODE_PACKAGE" https://go.microsoft.com/fwlink/?LinkID=620884
  fi
  # v1.1 wget -O "/tmp/$VSCODE_PACKAGE" https://az764295.vo.msecnd.net/stable/def9e32467ad6e4f48787d38caf190acbfee5880/$VSCODE_PACKAGE
  #wget -O "/tmp/$VSCODE_PACKAGE" https://az764295.vo.msecnd.net/stable/809e7b30e928e0c430141b3e6abf1f63aaf55589/$VSCODE_PACKAGE
fi

cd /tmp

if [ -f "/tmp/$VSCODE_PACKAGE" ]; then
  sudo unzip $VSCODE_PACKAGE  

  VSCODE_EXTRACTED_FOLDER=`ls -dt VSCode* | grep -v '.zip' | head -1`
  sudo rsync -av "$VSCODE_EXTRACTED_FOLDER" /opt/vscode

if [ "i686" == "$PLATFORM" ]; then
  echo '[Desktop Entry]
Name=VSCODE IDE
Type=Application
Exec=/opt/vscode/VSCode-linux-ia32/bin/code
Icon=/opt/vscode/VSCode-linux-ia32/resources/app/resources/linux/code.png
Terminal=false
Comment=Integrated Development Environment
NoDisplay=false
Categories=Development;IDE;
Name[en]=VSCODE IDEA
' > vscode.desktop
else
  echo '[Desktop Entry]
Name=VSCODE IDE
Type=Application
Exec=/opt/vscode/VSCode-linux-x64/bin/code
Icon=/opt/vscode/VSCode-linux-x64/resources/app/resources/linux/code.png
Terminal=false
Comment=Integrated Development Environment
NoDisplay=false
Categories=Development;IDE;
Name[en]=VSCODE IDEA
' > vscode.desktop
fi

  sudo cp $VSCODE_EXTRACTED_FOLDER/resources/app/resources/linux/code.png /usr/share/pixmaps/
  sudo desktop-file-install vscode.desktop
  sudo rm -f vscode.desktop
  sudo rm -f /usr/local/bin/code
  if [ "i686" == "$PLATFORM" ]; then
    sudo ln -s /opt/vscode/VSCode-linux-ia32/bin/code /usr/local/bin/code
  else
    sudo ln -s /opt/vscode/VSCode-linux-x64/bin/code /usr/local/bin/code
  fi
else
  echo "error - installation of vscode ide failed"
  echo "You can use Ubuntu Make to download and install Visual Studio Code:"

  sudo add-apt-repository ppa:ubuntu-desktop/ubuntu-make
  sudo apt-get update
  sudo apt-get install ubuntu-make

  echo "Then install Visual Studio Code:"

  umake ide visual-studio-code

fi

