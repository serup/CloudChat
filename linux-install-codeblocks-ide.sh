# installing the code::blocks ide
PKG_OK=$(dpkg-query -W --showformat='${Status}\n' 2>&1 codeblocks* |grep "install ok installed")
if [ "" == "$PKG_OK" ]; then
	# install essentials
        sudo add-apt-repository ppa:damien-moore/codeblocks-stable
        sudo apt-get update
        sudy apt-get install codeblocks 
fi
