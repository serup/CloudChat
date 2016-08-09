#!/usr/bin bash
user=$(whoami)
echo $user
sudo touch /etc/security/limits.d/$user.conf
sudo chown $user /etc/security/limits.d/$user.conf
sudo echo "$user soft nofile 1048570" > /etc/security/limits.d/$user.conf
sudo echo "$user soft nproc  1048570" >> /etc/security/limits.d/$user.conf
