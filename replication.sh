#!/bin/bash 
sshpass -p "vagrant" scp -oStrictHostKeyChecking=no -rp /var/www/img/ vagrant@cloudchatmanager.com:/var/www/CloudChatManager/.
rm -f /var/www/img/*
