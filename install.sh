#!/usr/bin/env bash -l
echo "********************************************************"
echo "** Installing vagrant, puppetlabs, virtualbox, docker **"
echo "********************************************************"
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
PKG_OK=$(dpkg-query -W --showformat='${Status}\n' 2>&1 libboost* |grep "install ok installed")
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
PKG_OK=$(dpkg-query -W --showformat='${Status}\n' 2>&1 vagrant |grep "install ok installed")
if [ "" == "$PKG_OK" ]; then
  echo -n "- install vagrant "
  #sudo apt-get --force-yes --yes install vagrant 
  # info : http://www.kianmeng.org/2015/07/vagrant-173-and-virtualbox-50.html
  sudo apt-get install aria2
  #aria2c -x 4 https://dl.bintray.com/mitchellh/vagrant/vagrant_1.7.4_x86_64.deb
  aria2c -x 4 https://releases.hashicorp.com/vagrant/1.7.4/vagrant_1.7.4_x86_64.deb
  sudo dpkg -i vagrant_1.7.4_x86_64.deb
  vagrant version
  echo " - done."
else
  echo "- vagrant installed"
  echo "- destroy old setup, so new can run"
  vagrant destroy
  echo "- update vagrant box to newest version"
  vagrant box update
fi
VBOX_OK=$(vagrant box list|awk 'BEGIN {strtmp=$1} END {print $strtmp}')
if [ "" == "$VBOX_OK" ]; then
  echo "vbox not found - installing.."
  #vagrant box add ubuntu/trusty64 https://atlas.hashicorp.com/ubuntu/boxes/trusty64/versions/14.04/providers/virtualbox.box
  vagrant box add ubuntu/vivid32 
else
  echo "- vbox installed"
fi
PKG_OK=$(dpkg-query -W --showformat='${Status}\n' 2>&1 puppetlabs-release |grep "install ok installed")
if [ "" == "$PKG_OK" ]; then
#  echo "puppetlabs-release was not found, now it will be installed - please wait..."
  echo -n "- install puppetlabs-release "
  sudo apt-get install puppet-common
#  wget https://apt.puppetlabs.com/puppetlabs-release-trusty.deb
#  sudo dpkg -i puppetlabs-release-trusty.deb
   wget https://apt.puppetlabs.com/puppetlabs-release-pc1-vivid.deb && \
   sudo dpkg -i puppetlabs-release-pc1-vivid.deb && \
   sudo apt-get update 
  echo " - done."
else
  echo "- puppetlabs-release installed"
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
  sudo apt-get install curl 
  echo -n "- install sdkman"
  curl -s http://get.sdkman.io | bash
  echo -n "- init sdk"
  source $(pwd)/.sdkman/bin/sdkman-init.sh
  gvm version
  echo -n "- install grails"
  gvm install grails 
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

echo "******************************************************************************************************************"
echo "environment is now ready! you may run vagrant up and then vagrant up cloudchatmanager, vagrant up cloudchatclient"
echo "******************************************************************************************************************"
vagrant up
#vagrant up jenkins
#vagrant up backend
#vagrant up cloudchatmanager
#vagrant up cloudchatclient
