#!/bin/bash

        echo "Do you wish to append cluster nodes to hosts file? should be done first time or else it will not work [Yes=1|No=2]"
        select yn in "Yes" "No"; do
            case $yn in
                Yes )   echo "append the cluster nodes to your hosts file";
			cat ambari-vagrant/append-to-etc-hosts.txt >> /etc/hosts;
			exit;
                        echo "nodes appended to /etc/hosts file";
                        break;;
                No )    echo "nodes NOT appended to /etc/hosts file";
                        break;;
            esac
        done

