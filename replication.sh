#!/bin/bash 
mkdir .thumbs
# resize to a maximum of 200x200 pixels in .thumbs subdir
gm mogrify -output-directory .thumbs -resize 150x170 '/var/www/img/*.png'
# Final move resulting image back
mv .thumbs/*.png /var/www/img/.  
# Then transfer files to frontend server
#sshpass -p "<password>" scp -oStrictHostKeyChecking=no -rp /var/www/img/ cloud1scanvacom@77.75.165.130:/var/www/html/CloudChatManager/.
sshpass -p "vagrant" scp -oStrictHostKeyChecking=no -rp /var/www/img/ vagrant@cloudchatmanager.com:/var/www/html/CloudChatManager/. 
#sleep 1000
# Cleanup
rm -rf .thumbs 
rm -f /var/www/img/*
exec "$@"
