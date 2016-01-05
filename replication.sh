#!/bin/bash 
# First resize to a maximum of 200x200 pixels
mogrify -resize 200x200 *.jpg
# Then transfer files to frontend server
sshpass -p "vagrant" scp -oStrictHostKeyChecking=no -rp /var/www/img/ vagrant@cloudchatmanager.com:/var/www/CloudChatManager/.
sleep 1000
# Cleanup
rm -f /var/www/img/*
exec "$@"
