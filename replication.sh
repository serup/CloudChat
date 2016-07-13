#!/bin/bash 
# First resize to a maximum of 80x80 pixels
mogrify -resize 80x80 '/var/www/img/*.jpg'
# Then transfer files to frontend server
#sshpass -p "<password>" scp -oStrictHostKeyChecking=no -rp /var/www/img/ cloud1scanvacom@77.75.165.130:/var/www/html/CloudChatManager/.
sshpass -p "vagrant" scp -oStrictHostKeyChecking=no -rp /var/www/img/ vagrant@cloudchatmanager.com:/var/www/html/CloudChatManager/. 
#sleep 1000
# Cleanup
#rm -f /var/www/img/*
exec "$@"
