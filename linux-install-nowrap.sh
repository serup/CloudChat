#/usr/bin bash
echo "Installing nowrap script for truncating color filled text from git log"
cd ~/GitHub
git clone https://github.com/goodell/nowrap.git
cd -
cd ~/GitHub/nowrap
echo "change permisions"
chmod ugo+x nowrap
echo "copy to /usr/local/bin"
sudo cp nowrap /usr/local/bin/.
cd -
echo "Done."
