# Specify IDEA version here.  -- NB! MUST RUN as administrator
VSCODE_PACKAGE=VSCode-linux64.zip

wget -O "/tmp/$VSCODE_PACKAGE" https://az764295.vo.msecnd.net/public/0.10.1-release/$VSCODE_PACKAGE
cd /tmp

if [ -f "/tmp/$VSCODE_PACKAGE" ]; then
  sudo unzip $VSCODE_PACKAGE  

  VSCODE_EXTRACTED_FOLDER=`ls -dt VSCode* | head -1`
  sudo rsync -av "$VSCODE_EXTRACTED_FOLDER" /opt/vscode
  sudo cp /opt/vscode/VSCode-linux-x64/resources/app/resources/linux/vscode.png /usr/share/pixmaps/


  echo '[Desktop Entry]
Name=VSCODE IDEA
Type=Application
Exec=/opt/vscode/VSCode-linux-x64/Code
Icon=/opt/vscode/VSCode-linux-x64/resources/app/resources/linux/vscode.png
Terminal=false
Comment=Integrated Development Environment
NoDisplay=false
Categories=Development;IDE;
Name[en]=VSCODE IDEA
' > vscode.desktop

  sudo desktop-file-install vscode.desktop
  sudo rm -f vscode.desktop
  sudo rm -f /usr/local/bin/code
  sudo ln -s /opt/vscode/VSCode-linux-x64/Code /usr/local/bin/code
else
  echo "error"
fi

