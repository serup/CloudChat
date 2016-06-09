#!/bin/sh
# Specify IDE version here.  -- NB! MUST RUN as administrator - like this: sudo bash  ./linux-install-VSCODE-ide.sh

VSCODE_PACKAGE=VSCode-linux-x64-stable.zip
#wget -O "/tmp/$VSCODE_PACKAGE" https://az764295.vo.msecnd.net/stable/def9e32467ad6e4f48787d38caf190acbfee5880/$VSCODE_PACKAGE
#wget -O "/tmp/$VSCODE_PACKAGE" https://az764295.vo.msecnd.net/stable/809e7b30e928e0c430141b3e6abf1f63aaf55589/$VSCODE_PACKAGE

if [ -f "/tmp/$VSCODE_PACKAGE" ]; then
  echo "package exists already - no need to download"
else
  wget -O "/tmp/$VSCODE_PACKAGE" https://az764295.vo.msecnd.net/stable/def9e32467ad6e4f48787d38caf190acbfee5880/$VSCODE_PACKAGE
  #wget -O "/tmp/$VSCODE_PACKAGE" https://az764295.vo.msecnd.net/stable/809e7b30e928e0c430141b3e6abf1f63aaf55589/$VSCODE_PACKAGE
fi

cd /tmp

if [ -f "/tmp/$VSCODE_PACKAGE" ]; then
  sudo unzip $VSCODE_PACKAGE  

  VSCODE_EXTRACTED_FOLDER=`ls -dt VSCode* | head -1`
  sudo rsync -av "$VSCODE_EXTRACTED_FOLDER" /opt/vscode
  sudo cp $VSCODE_EXTRACTED_FOLDER/resources/app/resources/linux/code.png /usr/share/pixmaps/

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

  sudo desktop-file-install vscode.desktop
  sudo rm -f vscode.desktop
  sudo rm -f /usr/local/bin/code
  sudo ln -s /opt/vscode/VSCode-linux-x64/bin/code /usr/local/bin/code

else
  echo "error - installation of vscode ide failed"
  echo "You can use Ubuntu Make to download and install Visual Studio Code:"

  sudo add-apt-repository ppa:ubuntu-desktop/ubuntu-make
  sudo apt-get update
  sudo apt-get install ubuntu-make

  echo "Then install Visual Studio Code:"

  umake ide visual-studio-code

fi

