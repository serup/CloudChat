# installing the code::blocks ide
# if issues starting codeblocks, then try this:
# gksudo codeblocks
PKG_OK=$(dpkg-query -W --showformat='${Status}\n' 2>&1 codeblocks* |grep "install ok installed")
if [ "" == "$PKG_OK" ]; then
	# install essentials
        sudo add-apt-repository ppa:damien-moore/codeblocks-stable
        sudo apt-get update
        sudo apt-get install codeblocks 
        sudo cp codeblocks_projects/colour_themes.conf ~/.config/codeblocks/default.conf
fi
