#/usr/bin/env bash
echo "Installing nowrap script for truncating color filled text from git log"
mkdir -p ~/GitHub
cd ~/GitHub; git clone https://github.com/goodell/nowrap.git
echo "change permisions"
cd ~/GitHub/nowrap; chmod ugo+x nowrap
echo "copy to /usr/local/bin"
cd ~/GitHub/nowrap; sudo cp nowrap /usr/local/bin/.
echo "Done."
