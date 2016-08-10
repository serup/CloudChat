# https://www.digitalocean.com/community/tutorials/how-to-install-linux-apache-mysql-php-lamp-stack-on-ubuntu-16-04
echo "Installing Apache"
sudo apt-get update
sudo apt-get install apache2
#sudo ufw app list
sudo ufw app info "Apache Full"


