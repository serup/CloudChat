#!/bin/bash 
sshpass -p "vagrant" scp -rp /var/www/img/ vagrant@cloudchatmanager.com:/var/www/CloudChatManager/.
