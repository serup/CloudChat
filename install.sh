#!/usr/bin/env bash
cat DOPS_outline.txt
echo "********************************************************"
echo "** Installing vagrant, puppetlabs, virtualbox, docker **"
echo "********************************************************"
echo "MUST run as '. ./install.sh' or as 'sudo bash ./install.sh' otherwise it will fail !!!"

echo "setting up alias"
alias alert='notify-send --urgency=low -i "$([ $? = 0 ] && echo terminal || echo error)" "$(history|tail -n1|sed -e '\''s/^\s*[0-9]\+\s*//;s/[;&|]\s*alert$//'\'')"'
alias egrep='egrep --color=auto'
alias ff='find . -type f -name '
alias fgrep='fgrep --color=auto'
alias findfile='find . -type f -name '
#alias gitlog='git log --pretty=format:"%C(yellow)%h %Cred%ad %Cblue%an%Cgreen%d %Creset%s" --date=short'
alias gitlog='git log --graph --decorate --abbrev-commit --all --oneline --color | nowrap | head -$(($(tput lines) - 1))' 
alias gitshow='function _blah(){ git show "$(git annotate $1 | grep $2 | head -1| cut -f1)"; };_blah'
alias grep='grep --color=auto'
alias gvm='sdk'
alias l='ls -CF'
alias la='ls -A'
alias ll='ls -alF'
alias ls='ls --color=auto'
alias vimstart='vim --cmd "let g:startify_disable_at_vimenter = 1" -S Session.vim'
# nb! use '\'' for each '  inside an alias
alias ks='function _blabla(){ kill "$(ps -aux|grep $1|head -1|awk '\''{print $2}'\'')"; }; _blabla'

echo "Setup ULIMIT -- IMPORTANT - this is needed to avoid too many handles or processes, which will freeze system"
bash setupUlimit.sh

function set-title() {
  if [[ -z "$ORIG" ]]; then
    ORIG=$PS1
  fi
  TITLE="\[\e]2;$@\a\]"
  #PS1=${ORIG}${TITLE}
  #export PS1='\e[01;33m\w>\[\033[00m\] '
  export PS1='\[\033[01;32m\]\u:\[\033[00m\]\W\$ '

  echo "put PS1 statement in ~/.bashrc : "
  #export PS1='${debian_chroot:+($debian_chroot)}\[\033[01;32m\]\u@\[\033[00m\]:\[\033[01;34m\]\w\[\033[00m\]\$ '
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
PKG_OK=$(dpkg-query -W --showformat='${Status}\n' 2>&1 apt-fast |grep "install ok installed")
if [ "" == "$PKG_OK" ]; then
  echo -n "- install apt-fast on ubuntu, to use for speeding up apt-get install's "
  sudo add-apt-repository ppa:saiarcot895/myppa
  sudo apt-get update
  sudo apt-get install -yq apt-fast
  echo " - done."
else
  echo "- apt-fast already installed"
fi
JQ_OK=$(dpkg --get-selections | grep -v deinstall|grep jq)
if [ "" == "$JQ_OK" ]; then
  echo -n "- install jq - for handling json files inside batch"
  sudo apt-fast install -yq  jq
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
  sudo apt-fast install -yq oracle-java8-installer
  sudo apt-fast install -yq oracle-java8-set-default
  java -version
  echo " - done."
else
  echo "- oracle-java already installed"
fi
PKG_OK=$(dpkg-query -W --showformat='${Status}\n' 2>&1 gpointing* |grep "install ok installed")
if [ "" == "$PKG_OK" ]; then
  echo -n "- install gpointing-device-settings on ubuntu, to enable middlemouse button copy / past "
  sudo apt-fast install -yq gpointing-device-settings
  echo " - done."
else
  echo "- gpointing already installed"
fi
PKG_OK=$(dpkg-query -W --showformat='${Status}\n' 2>&1 *stdc* |grep "install ok installed")
if [ "" == "$PKG_OK" ]; then
  echo -n "- install lib32stdc on ubuntu, to use for mksdcard SDK tool in android-studio "
  sudo apt-fast install -yq lib32stdc++6 
  echo " - done."
else
  echo "- lib32stdc already installed"
fi
PKG_OK=$(dpkg-query -W --showformat='${Status}\n' 2>&1 *libxml2-utils* |grep "install ok installed")
if [ "" == "$PKG_OK" ]; then
  echo -n "- install xmllint on ubuntu, to use for pretty format xml output; xmllint --format <file> "
  sudo apt-fast install -yq libxml2-utils 
  echo " - done."
else
  echo "- xmllint already installed"
fi
PKG_OK=$(dpkg-query -W --showformat='${Status}\n' 2>&1 gitk* |grep "install ok installed")
if [ "" == "$PKG_OK" ]; then
  echo -n "- install gitk on ubuntu, to use for visualizing git repos "
  sudo apt-fast install -yq gitk 
  echo " - done."
else
  echo "- gitk already installed"
fi
PKG_OK=$(dpkg-query -W --showformat='${Status}\n' 2>&1 kdiff3* |grep "install ok installed")
if [ "" == "$PKG_OK" ]; then
  echo -n "- install kdiff3 on ubuntu, to use when comparing files "
  sudo apt-fast install -yq kdiff3 
  echo " - done."
else
  echo "- kdiff3 already installed"
fi
PKG_OK=$(dpkg-query -W --showformat='${Status}\n' 2>&1 wireshark |grep "install ok installed")
if [ "" == "$PKG_OK" ]; then
  echo -n "- install wireshark on ubuntu, to use when monitoring tcp trafic "
  sudo apt-fast install -yq wireshark 
  echo " - done."
else
  echo "- wireshark already installed"
fi
PKG_OK=$(dpkg-query -W --showformat='${Status}\n' 2>&1 vim* |grep "install ok installed")
if [ "" == "$PKG_OK" ]; then
  echo "install pathogen.vim"
  mkdir -p ~/.vim/autoload ~/.vim/bundle && \
  echo -n "- install curl"
  sudo apt-fast install -yq curl 
  curl -LSso ~/.vim/autoload/pathogen.vim https://tpo.pe/pathogen.vim
  echo -n "- install vim on ubuntu "
  sudo apt-fast install -yq vim 
  echo 'set mouse=a' >> ~/.vimrc 
  # maximize vertical and horizontal with <Ctrl-W>M and restore with <Ctrl-W>m
  echo 'noremap <C-W>M <C-W>\| <C-W>_'  >> ~/.vimrc 
  echo 'noremap <C-W>m <C-W>=' >> ~/.vimrc  
  echo 'execute pathogen#infect()' >> ~/.vimrc
  echo 'syntax on' >> ~/.vimrc
  echo 'filetype plugin indent on' >> ~/.vimrc
  # unfortunately links to helpfile does not work - so copy instead
  #sudo ln -s vimhelpfile_build_commands.txt ~/.vim/doc/build_commands.txt 
  sudo cp vimhelpfile_build_commands.txt ~/.vim/doc/build_commands.txt
  echo "- to setup vimhelpfiles run following command; :helptags ~/.vim/doc"
  sudo apt-fast install -yq vim-addon-manager
  vam install youcompleteme
  echo "NO need to use linux-install-vim-intellisense.sh script"
  echo "- install window-swap plugin"
  git clone https://github.com/wesQ3/vim-windowswap ~/.vim/bundle/vim-windowswap
  echo " - done."
else
  echo "- vim already installed - consider adding set mouse=a   ;to enable mouse handling of splits"
fi
PKG_OK=$(dpkg-query -W --showformat='${Status}\n' 2>&1 *brew* |grep "install ok installed")
if [ "" == "$PKG_OK" ]; then
  echo -n "- install brew on ubuntu : example: brew install ctags"
  sudo apt-fast install -yq linuxbrew-wrapper 
  brew install ctags
  sudo apt-fast install -yq exuberant-ctags
  echo "- add to .vimrc -- Plug 'craigemery/vim-autotag'"
  echo " - done."
else
  echo "- brew already installed"
fi
PKG_OK=$(dpkg-query -W --showformat='${Status}\n' 2>&1 sysstat |grep "install ok installed")
if [ "" == "$PKG_OK" ]; then
  echo -n "- install sysstat on ubuntu : example: iostat -d 1 10"
  sudo apt-fast install -yq sysstat
  echo " - done."
else
  echo "- sysstat already installed"
fi
PKG_OK=$(dpkg-query -W --showformat='${Status}\n' 2>&1 bikeshed |grep "install ok installed")
if [ "" == "$PKG_OK" ]; then
  echo -n "- install bikeshed on ubuntu, to use when purging old kernals - example: sudo purge-old-kernals --keep 3 "
  sudo apt-fast install -yq bikeshed 
  echo " - done."
else
  echo "- bikeshed already installed, to use when purging old kernals - example: sudo purge-old-kernals --keep 3 "
fi
PKG_OK=$(dpkg-query -W --showformat='${Status}\n' 2>&1 nmap |grep "install ok installed")
if [ "" == "$PKG_OK" ]; then
  echo -n "- install nmap on ubuntu "
  sudo apt-fast install -yq nmap 
  echo " - done."
else
  echo "- nmap already installed"
fi
PKG_OK=$(dpkg-query -W --showformat='${Status}\n' 2>&1 eclipse |grep "install ok installed")
if [ "" == "$PKG_OK" ]; then
  echo -n "- install eclipse on ubuntu "
  sudo apt-fast install -yq eclipse 
  echo " - done."
  sudo bash eclipse_plugin_install.sh
else
  echo "- eclipse already installed"
fi

PKG_OK=$(dpkg-query -W --showformat='${Status}\n' 2>&1 openjdk* |grep "install ok installed")
if [ "" == "$PKG_OK" ]; then
  echo -n "- install eclipse dependencies on ubuntu "
  sudo apt-fast install -yq openjdk-6-jdk openjdk-6-source openjdk-6-demo openjdk-6-doc openjdk-6-jre-headless openjdk-6-jre-lib 
  echo " - done."
else
  echo "- eclipse dependencies already installed"
fi

PKG_OK=$(dpkg-query -W --showformat='${Status}\n' 2>&1 libjpeg* |grep "install ok installed")
if [ "" == "$PKG_OK" ]; then
  echo -n "- install jpeg dependencies on ubuntu "
  sudo apt-fast install -yq libjpeg-dev 
  echo " - done."
else
  echo "- libjpeg-dev dependencies already installed"
fi


PKG_OK=$(dpkg-query -W --showformat='${Status}\n' 2>&1 scene* |grep "install ok installed")
if [ "" == "$PKG_OK" ]; then
  echo -n "- install javaFX Scene builder "
   echo "NB! IMPORTANT go manually in on following homepage [http://www.oracle.com/technetwork/java/javase/downloads/index-jsp-138363.html]  and press download then press the  Accept the Oracle Binary Code License Agreement for Java SE -- then paste into browser this URL:"
   echo "PLEASE manually download http://download.oracle.com/otn-pub/java/javafx_scenebuilder/2.0-b20/javafx_scenebuilder-2_0-linux-i586.deb"
   echo "then do following commands with downloaded file : "
   echo "sudo dpkg -i javafx_scenebuilder-2_0-linux-i586.deb"
   echo "sudo apt-fast install -yq  scenebuilder"
   echo "sudo apt-get update"
   #wget http://download.oracle.com/otn-pub/java/javafx_scenebuilder/2.0-b20/javafx_scenebuilder-2_0-linux-i586.deb && \
   #sudo dpkg -i javafx_scenebuilder-2_0-linux-i586.deb && \
   #sudo dpkg -i javafx_scenebuilder-2_0-linux-x64.deb && \
   # find it using this command: dpkg-query  -S scene*
   # then add in intellij under settings/language.../JavaFX/path to scenebuilder
   #   it could be here : /opt/JavaFXSceneBuilder2.0/JavaFXSceneBuilder2.0
   sudo apt-fast install -yq  scenebuilder
   #sudo apt-get update 
  echo " - done."
else
  echo "- JavaFX Scene builder already installed"
fi
PKG_OK=$(dpkg-query -W --showformat='${Status}\n' 2>&1 p7zip-rar* |grep "install ok installed")
if [ "" == "$PKG_OK" ]; then
  echo -n "- install p7zip-rar "
  sudo apt-fast install -yq p7zip-rar 
  echo " - done."
else
  echo "- p7zip-rar already installed"
fi

PKG_OK=$(dpkg-query -W --showformat='${Status}\n' 2>&1 p7zip-full* |grep "install ok installed")
if [ "" == "$PKG_OK" ]; then
  echo -n "- install p7zip-full "
  sudo apt-fast install -yq p7zip-full 
  echo " - done."
else
  echo "- p7zip already installed"
fi
PKG_OK=$(dpkg-query -W --showformat='${Status}\n' 2>&1 p7zip-rar* |grep "install ok installed")
if [ "" == "$PKG_OK" ]; then
  echo -n "- install p7zip-rar "
  sudo apt-fast install -yq p7zip-rar 
  echo " - done."
else
  echo "- p7zip-rar already installed"
fi
PKG_OK=$(dpkg-query -W --showformat='${Status}\n' 2>&1 lcov* |grep "install ok installed")
if [ "" == "$PKG_OK" ]; then
  echo -n "- install lcov "
  sudo apt-fast install -yq lcov 
  echo " - done."
else
  echo "- lcov already installed"
fi
PKG_OK=$(dpkg-query -W --showformat='${Status}\n' 2>&1 nodejs* |grep "install ok installed")
if [ "" == "$PKG_OK" ]; then
  echo -n "- install nodejs-legacy "
  sudo apt-fast install -yq nodejs-legacy 
  echo " - done."
else
  echo "- nodejs-legacy already installed"
fi
PKG_OK=$(dpkg-query -W --showformat='${Status}\n' 2>&1 pidgin* |grep "install ok installed")
if [ "" == "$PKG_OK" ]; then
  echo -n "- install pidgin "
  sudo apt-fast install -yq pidgin 
  echo " - done."
else
  echo "- pidgin already installed"
fi
PKG_OK=$(dpkg-query -W --showformat='${Status}\n' 2>&1 monodevelop-nunit* |grep "install ok installed")
if [ "" == "$PKG_OK" ]; then
  echo -n "- install monodevelop-nunit for C# on ubuntu "
  sudo apt-fast install -yq monodevelop-nunit 
  echo " - done."
else
  echo "- monodevelop-nunit already installed"
fi
PKG_OK=$(dpkg-query -W --showformat='${Status}\n' 2>&1 nuget* |grep "install ok installed")
if [ "" == "$PKG_OK" ]; then
  echo -n "- install monodevelop-nunit for C# on ubuntu "
  sudo apt-fast install -yq nuget 
  echo " - done."
else
  echo "- nuget already installed"
fi
PKG_OK=$(dpkg-query -W --showformat='${Status}\n' 2>&1 mono-complete* |grep "install ok installed")
if [ "" == "$PKG_OK" ]; then
  sudo apt-key adv --keyserver hkp://keyserver.ubuntu.com:80 --recv-keys 3FA7E0328081BFF6A14DA29AA6A19B38D3D831EF
  #echo "deb http://download.mono-project.com/repo/debian wheezy main" | sudo tee /etc/apt/sources.list.d/mono-xamarin.list
  echo "deb http://download.mono-project.com/repo/debian nightly main" | sudo tee /etc/apt/sources.list.d/mono-nightly.list
  sudo apt-get update
  echo -n "- install mono-complete for C# on ubuntu "
  sudo apt-fast install -yq mono-complete 
  echo " - done."
else
  echo "- mono-complete already installed"
fi
PKG_OK=$(dpkg-query -W --showformat='${Status}\n' 2>&1 libnunit-cil-dev* |grep "install ok installed")
if [ "" == "$PKG_OK" ]; then
  echo -n "- install libnunit-cil-dev for C# on ubuntu "
  sudo apt-fast install -yq libnunit-cil-dev 
  echo " - done."
else
  echo "- libnunit-cil-dev already installed"
fi
PKG_OK=$(dpkg-query -W --showformat='${Status}\n' 2>&1 monodevelop* |grep "install ok installed")
if [ "" == "$PKG_OK" ]; then
  echo -n "- install monodevelop for C# on ubuntu "
  sudo apt-fast install -yq monodevelop 
  echo " - done."
else
  echo "- monodevelop already installed"
fi

PKG_OK=$(dpkg-query -W --showformat='${Status}\n' 2>&1 python-pip* |grep "install ok installed")
if [ "" == "$PKG_OK" ]; then
  echo -n "- install python-pip "
  sudo apt-fast install -yq python-pip
  echo " - done."
else
  echo "- python-pip already installed"
fi

PKG_OK=$(dpkg-query -W --showformat='${Status}\n' 2>&1 python-fs* |grep "install ok installed")
if [ "" == "$PKG_OK" ]; then
  echo -n "- install python-fs "
  sudo apt-fast install -yq python-fs
  echo " - done."
else
  echo "- python-fs already installed"
fi


# NB! INCASE pip is broken then repair it like this:
# sudo easy_install --upgrade pip
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
#  sudo apt-fast install -yq monodevelop-python
#  echo " - done."
#else
#  echo "- monodevelop-python already installed"
#fi

# PT. NOT working...
#PIPPKG_OK=$(pip list | grep python-hdfs*)
#if [ "" == "$PIPPKG_OK" ]; then
#  echo -n "- install python-hdfs on ubuntu "
#  pip install python-hdfs 
#  echo " - done."
#else
#  echo "- python-hdfs already installed"
#fi

PIPPKG_OK=$(pip list | grep -i WebHDFS*)
if [ "" == "$PIPPKG_OK" ]; then
  echo -n "- install python webhdfs on ubuntu "
  sudo easy_install webhdfs 
  echo " - done."
  echo " - NB! if issues on quoting, then download https://github.com/kupferk/webhdfs-py : "
  echo "       git clone https://github.com/kupferk/webhdfs-py.git "
  echo "       , and run sudo python setup.py install"
else
  echo "- python webhdfs already installed"
fi


PKG_OK=$(dpkg-query -W --showformat='${Status}\n' 2>&1 libwebkit-c* |grep "install ok installed")
if [ "" == "$PKG_OK" ]; then
  echo -n "- install libwebkit-cil-dev for C# on ubuntu "
  sudo apt-fast install -yq libwebkit-cil-dev 
  echo " - done."
else
  echo "- libwebkit already installed"
fi
PKG_OK=$(dpkg-query -W --showformat='${Status}\n' 2>&1 bundler* |grep "install ok installed")
if [ "" == "$PKG_OK" ]; then
  echo -n "- install bundler "
  sudo apt-fast install -yq bundler 
  sudo bundle install
  echo " - done."
else
  echo "- bundler already installed"
fi
PKG_OK=$(dpkg-query -W --showformat='${Status}\n' 2>&1 rubygems* |grep "install ok installed")
if [ "" == "$PKG_OK" ]; then
  echo -n "- install rubygems "
  sudo apt-fast install -yq rubygems 
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
  sudo apt-fast install -yq ruby-full 
  echo " - done."
else
  echo "- ruby-full already installed"
fi
PKG_OK=$(dpkg-query -W --showformat='${Status}\n' 2>&1 rsync* |grep "install ok installed")
if [ "" == "$PKG_OK" ]; then
  echo -n "- install rsync "
  sudo apt-fast install -yq rsync 
  echo " - done."
else
  echo "- rsync already installed"
fi
PKG_OK=$(dpkg-query -W --showformat='${Status}\n' 2>&1 ssh* |grep "install ok installed")
if [ "" == "$PKG_OK" ]; then
  echo -n "- install ssh "
  sudo apt-fast install -yq ssh 
  echo " - done."
else
  echo "- ssh already installed"
fi

PKG_OK=$(dpkg-query -W --showformat='${Status}\n' 2>&1 sshpass* |grep "install ok installed")
if [ "" == "$PKG_OK" ]; then
  echo -n "- install sshpass "
  sudo apt-fast install -yq sshpass
  echo " - done."
else
  echo "- sshpass already installed"
fi

PKG_OK=$(dpkg-query -W --showformat='${Status}\n' 2>&1 taskwarrior* |grep "install ok installed")
if [ "" == "$PKG_OK" ]; then
  echo -n "- install taskwarrior "
  sudo apt-fast install -yq taskwarrior
  echo " - done."
else
  echo "- taskwarrior already installed"
fi
PKG_OK=$(dpkg-query -W --showformat='${Status}\n' 2>&1 xslt* |grep "install ok installed")
if [ "" == "$PKG_OK" ]; then
  echo -n "- install xsltproc "
  sudo apt-fast install -yq xsltproc
  echo " - done."
else
  echo "- xsltproc already installed"
fi
PKG_OK=$(dpkg-query -W --showformat='${Status}\n' 2>&1 g++* |grep "install ok installed")
if [ "" == "$PKG_OK" ]; then
  # http://stackoverflow.com/questions/15359589/cout-is-not-a-member-of-std-cout-was-not-declared-in-this-scope
  sudo apt-get install -yq build-essential g++-multilib
  echo -n "- install g++ "
  echo " - done."
else
  echo "- g++ already installed"
fi
PKG_OK=$(dpkg-query -W --showformat='${Status}\n' 2>&1 g++* |grep "install ok installed")
if [ "" == "$PKG_OK" ]; then
  echo -n "- install g++ "
  sudo apt-fast install -yq g++ 
  echo " - done."
else
  echo "- g++ already installed"
fi

PKG_OK=$(dpkg-query -W --showformat='${Status}\n' 2>&1 npm* |grep "install ok installed")
if [ "" == "$PKG_OK" ]; then
  echo -n "- install npm "
  sudo apt-fast install -yq npm 
  echo " - done."
else
  echo "- npm already installed"
fi

PKG_OK=$(dpkg-query -W --showformat='${Status}\n' 2>&1 lcov* |grep "install ok installed")
if [ "" == "$PKG_OK" ]; then
  echo "install LCOV for code coverage"
  sudo apt-get install -yq lcov
  echo " - done."
else
  echo "- already installed"
fi

PKG_OK=$(dpkg-query -W --showformat='${Status}\n' 2>&1 nodejs* |grep "install ok installed")
if [ "" == "$PKG_OK" ]; then
  echo "fetch nodejs"
  sudo apt-get install -yq nodejs-legacy
  echo " - done."
else
  echo "- already installed"
fi

PKG_OK=$(dpkg-query -W --showformat='${Status}\n' 2>&1 libboost-all* |grep "install ok installed")
if [ "" == "$PKG_OK" ]; then
  echo -n "- install libboost "
  sudo apt-fast install -yq libboost-all-dev 
  echo "- install missing hpp file -- ignored until decided weather or not to use gil extension in backend"
  #sudo mkdir -p /usr/include/boost/gil/extension/numeric; sudo cp pixel_numeric_operations.hpp /usr/include/boost/gil/extension/numeric/.
  #sudo cp channel_numeric_operations.hpp /usr/include/boost/gil/extension/numeric/.
  #sudo cp affine.hpp /usr/include/boost/gil/extension/numeric/.
  #echo " - done."
else
  echo "- libbost already installed"
  echo "- install missing hpp file"
  #sudo mkdir -p /usr/include/boost/gil/extension/numeric; sudo cp pixel_numeric_operations.hpp /usr/include/boost/gil/extension/numeric/.
  #sudo cp channel_numeric_operations.hpp /usr/include/boost/gil/extension/numeric/.
  #sudo cp affine.hpp /usr/include/boost/gil/extension/numeric/.
 fi
PKG_OK=$(dpkg-query -W --showformat='${Status}\n' 2>&1 virtualbox |grep "install ok installed")
if [ "" == "$PKG_OK" ]; then
  echo -n "- install Virtualbox -- PARAMOUNT DO NOT HAVE SECURE BOOT ENABLED otherwise install of virtualbox will fail"

   wget http://download.virtualbox.org/virtualbox/5.0.20/virtualbox-5.0_5.0.20-106931~Ubuntu~xenial_amd64.deb && \
   sudo dpkg -i virtualbox-5.0_5.0.20-106931~Ubuntu~xenial_amd64.deb && \
   sudo apt-get update 
   sudo wget -q https://www.virtualbox.org/download/oracle_vbox_2016.asc -O- | sudo apt-key add -
   sudo wget -q https://www.virtualbox.org/download/oracle_vbox.asc -O- | sudo apt-key add -
   sudo apt-get update
   sudo apt-get install -yq virtualbox-5.0
else
  echo "- Virtualbox installed"
fi

PKG_OK=$(dpkg-query -W --showformat='${Status}\n' 2>&1 maven* |grep "install ok installed")
if [ "" == "$PKG_OK" ]; then
  echo -n "- install maven"
   sudo apt-fast install -yq maven 
else
  echo "- maven installed"
fi

PKG_OK=$(dpkg-query -W --showformat='${Status}\n' 2>&1 autoconf* |grep "install ok installed")
if [ "" == "$PKG_OK" ]; then
  echo -n "- install autoconf"
   sudo apt-fast install -yq autoconf
else
  echo "- autoconf installed"
fi


PKG_OK=$(dpkg-query -W --showformat='${Status}\n' 2>&1 virtualbox-guest* |grep "install ok installed")
if [ "" == "$PKG_OK" ]; then
  echo -n "- install Virtualbox guest addition "
  #http://download.virtualbox.org/virtualbox/5.0.16/VBoxGuestAdditions_5.0.16.iso
  sudo apt-fast install -yq virtualbox-guest-additions-iso
  sudo mkdir -p /media/VirtualBoxGuestAdditions
  sudo mount -t iso9660 -o loop /usr/share/virtualbox/VBoxGuestAdditions.iso /media/VirtualBoxGuestAdditions/
  sudo /media/VirtualBoxGuestAdditions/VBoxLinuxAdditions.run
  # vagrant-vbguest 0.10.1 requirement
  if [ ! -f /etc/init.d/vboxadd ]
  then
	vboxadd=$(find /opt -name vboxadd | head -n 1)
	if [ -f "$vboxadd" ]
	then
		sudo ln -s $vboxadd /etc/init.d
	fi
  fi
  sudo /etc/init.d/vboxadd setup
  echo " - done."
else
  echo "- Virtualbox guest addition installed"
fi

PKG_OK=$(dpkg-query -W --showformat='${Status}\n' 2>&1 puppet-co* |grep "install ok installed")
if [ "" == "$PKG_OK" ]; then
   echo -n "- install puppetlabs-release "
   sudo apt-fast install -yq puppet-common
   wget https://apt.puppetlabs.com/puppetlabs-release-pc1-vivid.deb && \
   sudo dpkg -i puppetlabs-release-pc1-vivid.deb && \
   sudo apt-get update 
  echo " - done."
else
  echo "- puppetlabs-release installed"
fi
MODULE_OK=$(puppet module list --modulepath ./puppet/trunk/environments/devtest/modules | grep roidelapluie-gerrit*)
if [ "" == "$MODULE_OK" ]; then
  echo -n "- install datacentred-gerrit"
  puppet module install roidelapluie-gerrit  --modulepath ./puppet/trunk/environments/devtest/modules
  echo " - done."
else
  echo "- roidelapluie-gerrit puppet module installed"
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

MODULE_OK=$(puppet module list --modulepath ./hadoop_projects/hadoopServices/modules | grep vzach-ambari*)
if [ "" == "$MODULE_OK" ]; then
  echo -n "- install vzach-ambari puppet module"
  puppet module install vzach-ambari --modulepath ./hadoop_projects/hadoopServices/modules
  echo " - done."
else
  echo "- vzach-ambari puppet module installed"
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
  sudo apt-fast install -yq curl 
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
  sudo apt-fast install -yq imagemagick
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
  echo -n " - do you want to cleanup nodes (y/n)? "
  read answer
  if echo "$answer" | grep -iq "^y" ;then
   echo "- destroy old setup, so new can run"
   vagrant destroy
   echo "- update vagrant box to newest version"
   vagrant box update
  else
   echo "- maintain old nodes "
  fi
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
  vagrant box add mwe/vivid32 mwe/vivid32 
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
