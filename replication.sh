#!/bin/bash 
sshpass -p "vagrant" scp -oStrictHostKeyChecking=no -rp /var/www/img/ vagrant@cloudchatmanager.com:/var/www/CloudChatManager/.
sleep 1000
rm -f /var/www/img/*
