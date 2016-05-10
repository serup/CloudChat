# Specify STUDIO version here.  -- NB! MUST RUN as administrator
ANDROID_STUDIO_PACKAGE=android-studio-ide-143.2790544-linux.zip 

# Assuming we already did this during the previous steps:
PKG_OK=$(dpkg-query -W --showformat='${Status}\n' 2>&1 oracle-java8-installer |grep "install ok installed")
if [ "" == "$PKG_OK" ]; then
# install essentials
        sudo add-apt-repository ppa:webupd8team/java
        sudo apt-get update
        sudo apt-get install oracle-java7-installer oracle-java8-installer 
fi

wget -O "/tmp/$ANDROID_STUDIO_PACKAGE" https://dl.google.com/dl/android/studio/ide-zips/2.1.0.9/$ANDROID_STUDIO_PACKAGE
cd /tmp

if [ -f "/tmp/$ANDROID_STUDIO_PACKAGE" ]; then
  sudo unzip "$ANDROID_STUDIO_PACKAGE" -d /opt

  STUDIO_EXTRACTED_FOLDER=`ls -dt android-studio* | head -1`
  sudo rsync -av "$STUDIO_EXTRACTED_FOLDER" /opt/android-studio
  sudo cp /opt/android-studio/bin/studio.png /usr/share/pixmaps/

  echo '[Desktop Entry]
Name=Android Studio
Type=Application
Exec="/opt/android-studio/bin/studio.sh" %f
Terminal=false
Icon=/opt/android-studio/bin/studio.png
Comment=Integrated Development Environment
NoDisplay=false
StartupNotify=true
StartupWMClass=android-studio
Categories=Development;IDE;
Name[en]=Android Studio
' > androidstudio.desktop

  sudo desktop-file-install androidstudio.desktop
  sudo rm -f androidstudio.desktop
  sudo rm -f /usr/local/bin/studio
  sudo ln -s /opt/android-studio/bin/studio.sh /usr/local/bin/studio
fi

