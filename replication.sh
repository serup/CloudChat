#!/bin/bash 
# First move image to new tmp area
mkdir -p /var/tmp/mogrifyTemp
cp /var/www/img/*.jpg /var/tmp/mogrifyTemp/.
# Then resize to a maximum of 80x80 pixels
mogrify -resize 150x170 '/var/tmp/mogrifyTemp/*.jpg'
# Final move resulting image back
cp /var/tmp/mogrifyTemp/*.jpg /var/www/img/.  
# cleanup tmp area
rm -rf /var/tmp/mogrifyTemp
# Then transfer files to frontend server
#sshpass -p "<password>" scp -oStrictHostKeyChecking=no -rp /var/www/img/ cloud1scanvacom@77.75.165.130:/var/www/html/CloudChatManager/.
sshpass -p "vagrant" scp -oStrictHostKeyChecking=no -rp /var/www/img/ vagrant@cloudchatmanager.com:/var/www/html/CloudChatManager/. 
#sleep 1000
# Cleanup
rm -f /var/www/img/*
exec "$@"
