#!/bin/bash 
# First resize to a maximum of 200x200 pixels
mogrify -resize 200x200 *.jpg
# Then transfer files to frontend server
#did not work -- sshpass -p "<password>" scp -rp /var/www/img/ cloud1scanvacom@77.75.165.130:/var/www/html/CloudChatManager/.
#sshpass -p "<password>" scp -oStrictHostKeyChecking=no -rp /var/www/img/* cloud1scanvacom@77.75.165.130:/var/www/html/CloudChatManager/img/.
sshpass -p "vagrant" scp -oStrictHostKeyChecking=no -rp /var/www/img/ vagrant@cloudchatmanager.com:/var/www/html/CloudChatManager/. 
#sleep 1000
# Cleanup
rm -f /var/www/img/*
exec "$@"
