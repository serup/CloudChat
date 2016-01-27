# Specify IDEA version here.
IDEA_PACKAGE=ideaIC-15.0.2.tar.gz

# Assuming we already did this during the previous steps:
PKG_OK=$(dpkg-query -W --showformat='${Status}\n' 2>&1 oracle-java8-installer |grep "install ok installed")
<<<<<<< HEAD
if ["" == "$PKG_OK" ]; then
# install essentials
	sudo add-apt-repository ppa:webupd8team/java
	sudo apt-get update
	sudy apt-get install oracle-java7-installer oracle-java8-installer
=======
if [ "" == "$PKG_OK" ]; then
# install essentials
        sudo add-apt-repository ppa:webupd8team/java
        sudo apt-get update
        sudy apt-get install oracle-java7-installer oracle-java8-installer
>>>>>>> serup
fi

if [ -d "/opt/idea" ]; then
   echo "/opt/idea ALREADY exists - please remove first before running this again, and if you want a reinstall, then remove hidden file in home directory .ideaIC15 and then delete installation directory for intellij"
else

if [ -f "/tmp/$IDEA_PACKAGE" ]; then
  cd /tmp
  tar -xzvf "$IDEA_PACKAGE"
else
  wget -O "/tmp/$IDEA_PACKAGE" http://download-cf.jetbrains.com/idea/$IDEA_PACKAGE
  cd /tmp
  if [ -f "/tmp/$IDEA_PACKAGE" ]; then
 	tar -xzvf "$IDEA_PACKAGE"
fi

  IDEA_EXTRACTED_FOLDER=`ls -dt idea-IC* | head -1`
  sudo rsync -av "$IDEA_EXTRACTED_FOLDER/" /opt/idea/
  sudo cp /opt/idea/bin/idea.png /usr/share/pixmaps/

  echo '[Desktop Entry]
Name=IntelliJ IDEA
Type=Application
Exec=/opt/idea/bin/idea.sh
Terminal=false
Icon=/opt/idea/bin/idea.png
Comment=Integrated Development Environment
NoDisplay=false
Categories=Development;IDE;
Name[en]=IntelliJ IDEA
' > idea.desktop

  sudo desktop-file-install idea.desktop
  sudo rm -f idea.desktop
  sudo rm -f /usr/local/bin/idea
  sudo ln -s /opt/idea/bin/idea.sh /usr/local/bin/idea
fi

fi
