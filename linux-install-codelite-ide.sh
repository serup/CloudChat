
# Assuming we already did this during the previous steps:
PKG_OK=$(dpkg-query -W --showformat='${Status}\n' 2>&1 codelite* |grep "install ok installed")
if [ "" == "$PKG_OK" ]; then
	# install essentials
	sudo apt-key adv --fetch-keys http://repos.codelite.org/CodeLite.asc
	sudo apt-add-repository "deb http://repos.codelite.org/ubuntu/ $(lsb_release -sc) universe"
	sudo apt-get update
	sudo apt-get install codelite wxcrafter
else
	echo "codelite is Already installed - you could remove it like this: sudo apt-get purge codelite codelite-plugins"
fi

