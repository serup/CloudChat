#!/usr/bin/env bash -l
echo "********************************************************"
echo "** Installing vagrant, puppetlabs, virtualbox, docker **"
echo "********************************************************"

function set-title() {
  if [[ -z "$ORIG" ]]; then
    ORIG=$PS1
  fi
  TITLE="\[\e]2;$@\a\]"
  PS1=${ORIG}${TITLE}
}

set-title environment for DOPS ok

if [ ! -f ~/.gradle/gradle.properties ]; then
	echo "enabling the gradle deamon to make gradle builds faster"
	touch ~/.gradle/gradle.properties && echo "org.gradle.daemon=true" >> ~/.gradle/gradle.properties
fi

DIR=$(cd . && pwd)
export DOPS_PUPPET_PATH="$DIR""/puppet/trunk/environments/"
export DOCKER_PUPPET_PATH="$DIR""/puppet/trunk/environments/"
echo "setting DOCKER_PUPPET_PATH=$DOCKER_PUPPET_PATH"
echo $DOCKER_PUPPET_PATH > env_docker_puppet_path
mkdir -p $DOCKER_PUPPET_PATH
JQ_OK=$(dpkg --get-selections | grep -v deinstall|grep jq)
if [ "" == "$JQ_OK" ]; then
  echo -n "- install jq - for handling json files inside batch"
  sudo apt-get install jq
  echo " - done."
else
  echo "- jq installed"
fi
project="$1"
image=""
if [ "file=setup.config" == "$project" ] || [ "" == "$project" ]; then 
   echo "  ___________________________________________________________________ "
   echo "    Warning no docker PROJECT name - please add it as first parameter - like this '. ./install.sh skeleton'"
else
   image=$(cat setup.conf | jq --raw-output '.'"$project"'.image')
fi
if [ "docker-image-skeleton" == "$image" ]; then
   echo -n "- setup for docker images for skeleton "
   scp=$(cat setup.conf | jq --raw-output '.'"$project"'.scp')
   mkdir -p $DOCKER_PUPPET_PATH/devtest/modules/docker_images/
   scp $scp $DOCKER_PUPPET_PATH/devtest/modules/docker_images/
   if [ "$?" != 0 ]; then
      echo "- docker images not installed"
   else
      echo "- docker images installed"
   fi
else
   echo "- docker images NOT installed - please check setup.conf to see if it is correct"
fi
PKG_OK=$(dpkg-query -W --showformat='${Status}\n' 2>&1 oracle-java* |grep "install ok installed")
if [ "" == "$PKG_OK" ]; then
  echo -n "- install oracle-java on ubuntu "
  sudo add-apt-repository -y ppa:webupd8team/java
  sudo apt-get update
  sudo apt-get install -yq oracle-java8-installer
  sudo apt-get install -yq oracle-java8-set-default
  java -version
  echo " - done."
else
  echo "- oracle-java already installed"
fi
PKG_OK=$(dpkg-query -W --showformat='${Status}\n' 2>&1 nmap |grep "install ok installed")
if [ "" == "$PKG_OK" ]; then
  echo -n "- install nmap on ubuntu "
  sudo apt-get install -yq nmap 
  echo " - done."
else
  echo "- nmap already installed"
fi
PKG_OK=$(dpkg-query -W --showformat='${Status}\n' 2>&1 eclipse |grep "install ok installed")
if [ "" == "$PKG_OK" ]; then
  echo -n "- install eclipse on ubuntu "
  sudo apt-get install -yq eclipse 
  echo " - done."
  sudo bash eclipse_plugin_install.sh
else
  echo "- eclipse already installed"
fi

PKG_OK=$(dpkg-query -W --showformat='${Status}\n' 2>&1 openjdk* |grep "install ok installed")
if [ "" == "$PKG_OK" ]; then
  echo -n "- install eclipse dependencies on ubuntu "
  sudo apt-get install -yq openjdk-6-jdk openjdk-6-source openjdk-6-demo openjdk-6-doc openjdk-6-jre-headless openjdk-6-jre-lib 
  echo " - done."
else
  echo "- eclipse dependencies already installed"
fi

PKG_OK=$(dpkg-query -W --showformat='${Status}\n' 2>&1 scene* |grep "install ok installed")
if [ "" == "$PKG_OK" ]; then
  echo -n "- install javaFX Scene builder "
   echo "PLEASE manually download http://download.oracle.com/otn-pub/java/javafx_scenebuilder/2.0-b20/javafx_scenebuilder-2_0-linux-i586.deb"
   #wget http://download.oracle.com/otn-pub/java/javafx_scenebuilder/2.0-b20/javafx_scenebuilder-2_0-linux-i586.deb && \
   #sudo dpkg -i javafx_scenebuilder-2_0-linux-i586.deb && \
   #sudo dpkg -i javafx_scenebuilder-2_0-linux-x64.deb && \
   # find it using this command: dpkg-query  -S scene*
   # then add in intellij under settings/language.../JavaFX/path to scenebuilder
   sudo apt-get install scenebuilder
   sudo apt-get update 
  echo " - done."
else
  echo "- JavaFX Scene builder already installed"
fi
PKG_OK=$(dpkg-query -W --showformat='${Status}\n' 2>&1 p7zip-rar* |grep "install ok installed")
if [ "" == "$PKG_OK" ]; then
  echo -n "- install p7zip-rar "
  sudo apt-get install -yq p7zip-rar 
  echo " - done."
else
  echo "- p7zip-rar already installed"
fi

PKG_OK=$(dpkg-query -W --showformat='${Status}\n' 2>&1 p7zip-full* |grep "install ok installed")
if [ "" == "$PKG_OK" ]; then
  echo -n "- install p7zip-full "
  sudo apt-get install -yq p7zip-full 
  echo " - done."
else
  echo "- p7zip already installed"
fi
PKG_OK=$(dpkg-query -W --showformat='${Status}\n' 2>&1 p7zip-rar* |grep "install ok installed")
if [ "" == "$PKG_OK" ]; then
  echo -n "- install p7zip-rar "
  sudo apt-get install -yq p7zip-rar 
  echo " - done."
else
  echo "- p7zip-rar already installed"
fi
PKG_OK=$(dpkg-query -W --showformat='${Status}\n' 2>&1 lzma* |grep "install ok installed")
if [ "" == "$PKG_OK" ]; then
  echo -n "- install LZMA "
  sudo apt-get install -yq LZMA 
  echo " - done."
else
  echo "- LZMA already installed"
fi
PKG_OK=$(dpkg-query -W --showformat='${Status}\n' 2>&1 lcov* |grep "install ok installed")
if [ "" == "$PKG_OK" ]; then
  echo -n "- install lcov "
  sudo apt-get install -yq lcov 
  echo " - done."
else
  echo "- lcov already installed"
fi
PKG_OK=$(dpkg-query -W --showformat='${Status}\n' 2>&1 nodejs* |grep "install ok installed")
if [ "" == "$PKG_OK" ]; then
  echo -n "- install nodejs-legacy "
  sudo apt-get install -yq nodejs-legacy 
  echo " - done."
else
  echo "- nodejs-legacy already installed"
fi
PKG_OK=$(dpkg-query -W --showformat='${Status}\n' 2>&1 pidgin* |grep "install ok installed")
if [ "" == "$PKG_OK" ]; then
  echo -n "- install pidgin "
  sudo apt-get install -yq pidgin 
  echo " - done."
else
  echo "- pidgin already installed"
fi
PKG_OK=$(dpkg-query -W --showformat='${Status}\n' 2>&1 monodevelop-nunit* |grep "install ok installed")
if [ "" == "$PKG_OK" ]; then
  echo -n "- install monodevelop-nunit for C# on ubuntu "
  sudo apt-get install -yq monodevelop-nunit 
  echo " - done."
else
  echo "- monodevelop-nunit already installed"
fi
PKG_OK=$(dpkg-query -W --showformat='${Status}\n' 2>&1 libnunit-cil-dev* |grep "install ok installed")
if [ "" == "$PKG_OK" ]; then
  echo -n "- install libnunit-cil-dev for C# on ubuntu "
  sudo apt-get install -yq libnunit-cil-dev 
  echo " - done."
else
  echo "- libnunit-cil-dev already installed"
fi
PKG_OK=$(dpkg-query -W --showformat='${Status}\n' 2>&1 monodevelop* |grep "install ok installed")
if [ "" == "$PKG_OK" ]; then
  echo -n "- install monodevelop for C# on ubuntu "
  sudo apt-get install -yq monodevelop 
  echo " - done."
else
  echo "- monodevelop already installed"
fi

PKG_OK=$(dpkg-query -W --showformat='${Status}\n' 2>&1 python-pip* |grep "install ok installed")
if [ "" == "$PKG_OK" ]; then
  echo -n "- install python-pip "
  sudo apt-get install -yq python-pip
  echo " - done."
else
  echo "- python-pip already installed"
fi

PIPPKG_OK=$(pip list| grep "http")
if [ "" == "$PIPPKG_OK" ]; then
  echo -n "- install python package http "
  sudo pip install http 
  echo " - done."
else
  echo "- python package http already installed"
fi


# somehow not working on ubuntu 15.04 vivid  -- use pyCharm in intellij instead : https://confluence.jetbrains.com/display/PYH/PyCharm+IDE+and+Python+Plugin+for+IntelliJ+IDEA
# install the http://plugins.jetbrains.com/plugin/631
#
#PKG_OK=$(dpkg-query -W --showformat='${Status}\n' 2>&1 monodevelop-python* |grep "install ok installed")
#if [ "" == "$PKG_OK" ]; then
#  echo -n "- install monodevelop for python on ubuntu "
#  #sudo sh -c 'echo deb http://us.archive.ubuntu.com/ubuntu vivid main universe > /etc/apt/sources.list.d/monodevelop-python.list' 
#  sudo add-apt-repository ppa:keks9n/monodevelop-latest  
#  sudo apt-get update -yq
#  sudo apt-get install -yq monodevelop-python
#  echo " - done."
#else
#  echo "- monodevelop-python already installed"
#fi

PKG_OK=$(dpkg-query -W --showformat='${Status}\n' 2>&1 libwebkit-c* |grep "install ok installed")
if [ "" == "$PKG_OK" ]; then
  echo -n "- install libwebkit-cil-dev for C# on ubuntu "
  sudo apt-get install -yq libwebkit-cil-dev 
  echo " - done."
else
  echo "- libwebkit already installed"
fi
PKG_OK=$(dpkg-query -W --showformat='${Status}\n' 2>&1 bundler* |grep "install ok installed")
if [ "" == "$PKG_OK" ]; then
  echo -n "- install bundler "
  sudo apt-get install -yq bundler 
  sudo bundle install
  echo " - done."
else
  echo "- bundler already installed"
fi
PKG_OK=$(dpkg-query -W --showformat='${Status}\n' 2>&1 rubygems* |grep "install ok installed")
if [ "" == "$PKG_OK" ]; then
  echo -n "- install rubygems "
  sudo apt-get install -yq rubygems 
  echo " - done."
else
  echo "- rubygems already installed"
fi
JEKYLL_OK=$(jekyll -v|grep jekyll)
if [ "" == "$JEKYLL_OK" ]; then 
  echo -n "- install jekyll"
  sudo gem install jekyll
  echo " - done."
else
  echo "- jekyll already installed"
fi
PKG_OK=$(dpkg-query -W --showformat='${Status}\n' 2>&1 ruby-full* |grep "install ok installed")
if [ "" == "$PKG_OK" ]; then
  echo -n "- install ruby-full "
  sudo apt-get install -yq ruby-full 
  echo " - done."
else
  echo "- ruby-full already installed"
fi
PKG_OK=$(dpkg-query -W --showformat='${Status}\n' 2>&1 rsync* |grep "install ok installed")
if [ "" == "$PKG_OK" ]; then
  echo -n "- install rsync "
  sudo apt-get install -yq rsync 
  echo " - done."
else
  echo "- rsync already installed"
fi
PKG_OK=$(dpkg-query -W --showformat='${Status}\n' 2>&1 ssh* |grep "install ok installed")
if [ "" == "$PKG_OK" ]; then
  echo -n "- install ssh "
  sudo apt-get install -yq ssh 
  echo " - done."
else
  echo "- ssh already installed"
fi

PKG_OK=$(dpkg-query -W --showformat='${Status}\n' 2>&1 sshpass* |grep "install ok installed")
if [ "" == "$PKG_OK" ]; then
  echo -n "- install sshpass "
  sudo apt-get install -yq sshpass
  echo " - done."
else
  echo "- sshpass already installed"
fi

PKG_OK=$(dpkg-query -W --showformat='${Status}\n' 2>&1 taskwarrior* |grep "install ok installed")
if [ "" == "$PKG_OK" ]; then
  echo -n "- install taskwarrior "
  sudo apt-get install -yq taskwarrior
  echo " - done."
else
  echo "- taskwarrior already installed"
fi
PKG_OK=$(dpkg-query -W --showformat='${Status}\n' 2>&1 xslt* |grep "install ok installed")
if [ "" == "$PKG_OK" ]; then
  echo -n "- install xsltproc "
  sudo apt-get install -yq xsltproc
  echo " - done."
else
  echo "- xsltproc already installed"
fi
PKG_OK=$(dpkg-query -W --showformat='${Status}\n' 2>&1 g++* |grep "install ok installed")
if [ "" == "$PKG_OK" ]; then
  echo -n "- install g++ "
  sudo apt-get install -yq g++ 
  echo " - done."
else
  echo "- g++ already installed"
fi
PKG_OK=$(dpkg-query -W --showformat='${Status}\n' 2>&1 libboost-all* |grep "install ok installed")
if [ "" == "$PKG_OK" ]; then
  echo -n "- install libboost "
  sudo apt-get install -yq libboost-all-dev 
  echo " - done."
else
  echo "- libbost already installed"
fi
PKG_OK=$(dpkg-query -W --showformat='${Status}\n' 2>&1 virtualbox |grep "install ok installed")
if [ "" == "$PKG_OK" ]; then
  echo -n "- install Virtualbox "
  sudo apt-get --force-yes --yes install virtualbox 
  echo " - done."
else
  echo "- Virtualbox installed"
fi

PKG_OK=$(dpkg-query -W --showformat='${Status}\n' 2>&1 virtualbox-guest* |grep "install ok installed")
if [ "" == "$PKG_OK" ]; then
  echo -n "- install Virtualbox guest addition "
  sudo apt-get install -yq virtualbox-guest-additions-iso
  sudo mkdir -p /media/VirtualBoxGuestAdditions
  sudo mount -t iso9660 -o loop /usr/share/virtualbox/VBoxGuestAdditions.iso /media/VirtualBoxGuestAdditions/
  sudo /media/VirtualBoxGuestAdditions/VBoxLinuxAdditions.run
  sudo /etc/init.d/vboxadd setup
  echo " - done."
else
  echo "- Virtualbox guest addition installed"
fi
PKG_OK=$(dpkg-query -W --showformat='${Status}\n' 2>&1 puppet-co* |grep "install ok installed")
if [ "" == "$PKG_OK" ]; then
#  echo "puppetlabs-release was not found, now it will be installed - please wait..."
  echo -n "- install puppetlabs-release "
  sudo apt-get install -yq puppet-common
#  wget https://apt.puppetlabs.com/puppetlabs-release-trusty.deb
#  sudo dpkg -i puppetlabs-release-trusty.deb
   wget https://apt.puppetlabs.com/puppetlabs-release-pc1-vivid.deb && \
   sudo dpkg -i puppetlabs-release-pc1-vivid.deb && \
   sudo apt-get update 
  echo " - done."
else
  echo "- puppetlabs-release installed"
fi
MODULE_OK=$(puppet module list --modulepath ./puppet/trunk/environments/devtest/modules | grep maestrodev-bamboo*)
if [ "" == "$MODULE_OK" ]; then
  echo -n "- install maestrodev-bamboo and nexus"
  puppet module install maestrodev-bamboo  --modulepath ./puppet/trunk/environments/devtest/modules
  puppet module install maestrodev-wget --modulepath ./puppet/trunk/environments/devtest/modules 
  MODULE_OK=$(puppet module list --modulepath ./puppet/trunk/environments/devtest/modules | grep hubspot-nexus*)
  if [ "" == "$MODULE_OK" ]; then
     puppet module install hubspot-nexus --modulepath ./puppet/trunk/environments/devtest/modules  
  fi
  echo " - done."
else
  echo "- maestrodev-bamboo puppet module installed"
fi

#MODULE_OK=$(puppet module list --modulepath ./hadoop_projects/hadoopServices/modules | grep bcarpio-hadoop*)
#if [ "" == "$MODULE_OK" ]; then
#  echo -n "- install hadoop puppet module"
#  puppet module install bcarpio-hadoop --modulepath ./hadoop_projects/hadoopServices/modules
#  echo " - done."
#else
#  echo "- bcarpio-hadoop puppet module installed"
#fi

#MODULE_OK=$(puppet module list --modulepath ./hadoop_projects/hadoopServices/modules | grep viirya-hadoop*)
#if [ "" == "$MODULE_OK" ]; then
#  echo -n "- install hadoop puppet module"
#  puppet module install viirya-hadoop --modulepath ./hadoop_projects/hadoopServices/modules
#  echo " - done."
#else
#  echo "- viirya-hadoop based on bcarpio-hadoop puppet module installed"
#fi

#MODULE_OK=$(puppet module list --modulepath ./hadoop_projects/hadoopServices/modules | grep viirya-java*)
#if [ "" == "$MODULE_OK" ]; then
#  echo -n "- install viirya-java puppet module"
#  puppet module install viirya-java --modulepath ./hadoop_projects/hadoopServices/modules
#  echo " - done."
#else
#  echo "- viirya-java puppet module installed"
#fi


#MODULE_OK=$(puppet module list --modulepath ./hadoop_projects/hadoopServices/modules | grep puppetlabs-java*)
#if [ "" == "$MODULE_OK" ]; then
#  echo -n "- install puppetlabs-java puppet module"
#  puppet module install puppetlabs-java --modulepath ./hadoop_projects/hadoopServices/modules
#  echo " - done."
#else
#  echo "- puppetlabs-java puppet module installed"
#fi


MODULE_OK=$(puppet module list --modulepath ./puppet/trunk/environments/devtest/modules | grep hubspot-nexus*)
if [ "" == "$MODULE_OK" ]; then
  echo -n "- install nexus"
  puppet module install hubspot-nexus --modulepath ./puppet/trunk/environments/devtest/modules  
  echo " - done."
else
  echo "- nexus puppet module installed"
fi
MODULE_OK=$(puppet module list --modulepath ./puppet/trunk/environments/devtest/modules | grep puppetlabs-stdlib*)
if [ "" == "$MODULE_OK" ]; then
  echo -n "- install puppetlabs-stdlib"
  puppet module install puppetlabs-stdlib  --modulepath ./puppet/trunk/environments/devtest/modules
  echo " - done."
else
  echo "- puppetlabs-stdlib puppet module installed"
fi
MODULE_OK=$(puppet module list --modulepath ./puppet/trunk/environments/devtest/modules | grep puppetlabs-apache*)
if [ "" == "$MODULE_OK" ]; then
  echo -n "- install puppetlabs-apache"
  puppet module install puppetlabs-apache  --modulepath ./puppet/trunk/environments/devtest/modules
  echo " - done."
else
  echo "- puppetlabs-apache puppet module installed"
fi
MODULE_OK=$(puppet module list --modulepath ./puppet/trunk/environments/devtest/modules | grep jfryman-nginx*)
if [ "" == "$MODULE_OK" ]; then
  echo -n "- install  jfryman-nginx"
  puppet module install  jfryman-nginx --modulepath ./puppet/trunk/environments/devtest/modules
  echo " - done."
else
  echo "- pjfryman-nginx puppet module installed"
fi
MODULE_OK=$(puppet module list --modulepath ./puppet/trunk/environments/devtest/modules | grep maestrodev-cucumber*)
if [ "" == "$MODULE_OK" ]; then
  echo -n "- install puppet-cucumber"
  puppet module install maestrodev-cucumber --modulepath ./puppet/trunk/environments/devtest/modules
  echo " - done."
else
  echo "- maestrodev-cucumber puppet module installed"
fi
MODULE_OK=$(puppet module list --modulepath ./puppet/trunk/environments/devtest/modules | grep leonardothibes-jekyll)
if [ "" == "$MODULE_OK" ]; then
  echo -n "- install leonardothibes-jekyll"
  puppet module install leonardothibes-jekyll --modulepath ./puppet/trunk/environments/devtest/modules
  echo " - done."
else
  echo "- leonardothibes-jekyll puppet module installed"
fi
MODULE_OK=$(puppet module list --modulepath ./puppet/trunk/environments/devtest/modules | grep puppetlabs-stdlib)
if [ "" == "$MODULE_OK" ]; then
  echo -n "- install puppetlabs-stdlib"
  puppet module install puppetlabs/stdlib --modulepath ./puppet/trunk/environments/devtest/modules
  echo " - done."
else
  echo "- puppetlabs-stdlib puppet module installed"
fi

MODULE_OK=$(puppet module list --modulepath ./puppet/trunk/environments/devtest/modules | grep puppetlabs-ruby)
if [ "" == "$MODULE_OK" ]; then
  echo -n "- install puppetlabs-ruby"
  puppet module install puppetlabs-ruby  --modulepath ./puppet/trunk/environments/devtest/modules
  echo " - done."
else
  echo "- puppetlabs-ruby puppet module installed"
fi

SAZ_OK=$(puppet module list --modulepath ./puppet/trunk/environments/devtest/modules | grep saz-sudo)
if [ "" == "$SAZ_OK" ]; then
  echo -n "- install saz/sudo"
  puppet module install saz/sudo --modulepath ./puppet/trunk/environments/devtest/modules
  echo " - done."
else
  echo "- saz/sudo puppet module installed"
fi
DOCKER_OK=$(puppet module list --modulepath ./puppet/trunk/environments/devtest/modules | grep garethr-docker)
if [ "" == "$DOCKER_OK" ]; then
  echo -n "- install garethr/docker puppet module"
  puppet module install garethr/docker --modulepath ./puppet/trunk/environments/devtest/modules
  echo " - done."
else
  echo "- garethr/docker puppet module installed"
fi
SPRINGBOOT_OK=$(puppet module list --modulepath ./puppet/trunk/environments/devtest/modules | grep puppetlabs-java) 
if [ "" == "$SPRINGBOOT_OK" ]; then
#  echo -n "- install puppet-springboot puppet module"
#  puppet module install NERC-CEH/puppet-springboot --modulepath ./puppet/trunk/environments/devtest/modules
  echo -n "- install java puppet module"
  puppet module install puppetlabs/java --modulepath ./puppet/trunk/environments/devtest/modules
  echo -n "- install maven puppet module"
  puppet module install maestrodev/maven  --modulepath ./puppet/trunk/environments/devtest/modules
  echo -n "- install curl"
  sudo apt-get install -yq curl 
  echo -n "- install sdkman"
  curl -s http://get.sdkman.io | bash
  echo -n "- init sdk"
  source $(pwd)/.sdkman/bin/sdkman-init.sh
  sdk version
  echo -n "- install grails"
  sdk install grails < /dev/null
  echo " - done."
else
  echo "- puppet-springboot puppet module installed"
fi
GIT_OK=$(puppet module list --modulepath ./puppet/trunk/environments/devtest/modules | grep puppetlabs-git)
if [ "" == "$GIT_OK" ]; then
  echo -n "- install puppetlabs-git puppet module"
  puppet module install puppetlabs-git --modulepath ./puppet/trunk/environments/devtest/modules
  echo " - done."
else
  echo "- puppetlabs-git puppet module installed"
fi
NODEJS_OK=$(puppet module list --modulepath ./puppet/trunk/environments/devtest/modules | grep puppetlabs-nodejs)
if [ "" == "$NODEJS_OK" ]; then
  echo -n "- install puppetlabs-nodejs puppet module"
  puppet module install puppetlabs-nodejs --modulepath ./puppet/trunk/environments/devtest/modules
  echo " - done."
else
  echo "- puppetlabs-nodejs puppet module installed"
fi
BUILD_ESSENTIAL_OK=$(puppet module list --modulepath ./puppet/trunk/environments/devtest/modules | grep initforthe-build_essential)
if [ "" == "$BUILD_ESSENTIAL_OK" ]; then
  echo -n "- install initforthe-build_essential puppet module"
  puppet module install initforthe-build_essential --modulepath ./puppet/trunk/environments/devtest/modules
  echo " - done."
else
  echo "- initforthe-build_essential puppet module installed"
fi
INTELLIJ_OK=$(puppet module list --modulepath ./puppet/trunk/environments/devtest/modules | grep gini-idea)
if [ "" == "$INTELLIJ_OK" ]; then
  echo -n "- install gini-idea puppet module"
  puppet module install gini-idea --modulepath ./puppet/trunk/environments/devtest/modules
  echo " - done."
else
  echo "- gini-idea puppet module installed"
fi

# needed a swap file for compiling inside VM  see info here : http://www.cyberciti.biz/faq/linux-add-a-swap-file-howto/
SWAPFILE_OK=$(puppet module list --modulepath ./puppet/trunk/environments/devtest/modules | grep petems-swap_file)
if [ "" == "$SWAPFILE_OK" ]; then
  echo -n "- install petems-swap_file puppet module"
  puppet module install petems-swap_file --modulepath ./puppet/trunk/environments/devtest/modules
  echo " - done."
else
  echo "- petems-swap_file puppet module installed"
fi

if [ ! -f tmphost ]; then
    echo "add new hosts to hosts file"    
    cat /etc/hosts > tmphost
    # Configure /etc/hosts file
    sudo bash confighosts.sh
fi

# a community aka confluence, however free version - mediawiki
# make it possible for devs/users to communicate about the progress
MEDIAWIKI_OK=$(puppet module list --modulepath ./puppet/trunk/environments/devtest/modules | grep martasd-mediawiki)
if [ "" == "$MEDIAWIKI_OK" ]; then
  echo -n "- install martasd-mediawiki puppet module"
  puppet module install martasd-mediawiki  --modulepath ./puppet/trunk/environments/devtest/modules 
  echo " - done."
else
  echo "- martasd-mediawiki puppet module installed"
fi

# install mogrify - to resize extracted profile images, thus making transfer to javascript client performance wice faster
MOGRIFY_OK=$(dpkg-query -W --showformat='${Status}\n' 2>&1 imagemagick |grep "install ok installed")
if [ "" == "$MOGRIFY_OK" ]; then
  echo -n "- install imagemagick to make use of mogrify "
  sudo apt-get install -yq imagemagick
  echo " - done."
else
  echo "- mogrify installed"
fi


PKG_OK=$(cd; dpkg-query -W --showformat='${Status}\n' 2>&1 vagrant* |grep "install ok installed")
if [ "" == "$PKG_OK" ]; then
  echo -n "- install vagrant "
  sudo apt-get install -yq aria2
  PLATFORM=$(uname -i)
  if [ "i686" == "$PLATFORM" ]; then
    aria2c -x 4 https://releases.hashicorp.com/vagrant/1.7.4/vagrant_1.7.4_i686.deb
    sudo dpkg -i vagrant_1.7.4_i686.deb
  else
    #aria2c -x 4 https://dl.bintray.com/mitchellh/vagrant/vagrant_1.7.4_x86_64.deb
    aria2c -x 4 https://releases.hashicorp.com/vagrant/1.7.4/vagrant_1.7.4_x86_64.deb
    sudo dpkg -i vagrant_1.7.4_x86_64.deb
  fi
  vagrant version
  echo " - done."
else
  echo "- vagrant installed"
  echo "- destroy old setup, so new can run"
  vagrant destroy
  echo "- update vagrant box to newest version"
  vagrant box update
fi

PLUGIN_OK=$(vagrant plugin list|grep vagrant-vbguest) 
if [ "" == "$PLUGIN_OK" ]; then
  echo -n "- install vagrant plugin vbguest"
  sudo vagrant plugin install vagrant-vbguest
  echo " - done."
else
  echo "- vagrant plugin vbguest installed"
fi
VBOX_OK=$(vagrant box list|awk 'BEGIN {strtmp=$1} END {print $strtmp}')
if [ "" == "$VBOX_OK" ]; then
  echo "vbox not found - installing.."
  #vagrant box add ubuntu/trusty64 https://atlas.hashicorp.com/ubuntu/boxes/trusty64/versions/14.04/providers/virtualbox.box
  vagrant box add ubuntu/vivid32 
else
  echo "- vbox installed"
fi
echo "******************************************************************************************************************"
echo "environment is now ready! you may run vagrant up and then vagrant up cloudchatmanager, vagrant up cloudchatclient"
echo "******************************************************************************************************************"
#vagrant up
#vagrant up jenkins
#vagrant up backend
#vagrant up cloudchatmanager
#vagrant up cloudchatclient
