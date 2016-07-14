#!/bin/bash 
mkdir -p /var/www/.thumbs
cd /var/www/img
# resize to a maximum of 200x200 pixels in .thumbs subdir
gm mogrify -output-directory /var/www/.thumbs -resize 150x170 "*"
# Then transfer files to frontend server
#sshpass -p "<password>" scp -oStrictHostKeyChecking=no -rp /var/www/.thumbs/* cloud1scanvacom@77.75.165.130:/var/www/html/CloudChatManager/img/
sshpass -p "vagrant" scp -oStrictHostKeyChecking=no -rp /var/www/.thumbs/* vagrant@cloudchatmanager.com:/var/www/html/CloudChatManager/img/
#sleep 1000
# Cleanup
#rm -rf /var/www/.thumbs 
#rm -f /var/www/img/*
exec "$@"
