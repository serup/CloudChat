#!/bin/bash
# run as:  sudo ./install_ambari_cluster.sh 

if [ -d "ambari-vagrant" ]; then
	echo "ambari-vagrant cluster already installed"
else
	echo "clone ambari-vagrant from github"
	git clone https://github.com/u39kun/ambari-vagrant.git
	# insert lines in Vagrantfile - should only be done once
	echo "add virtualbox names to nodes"
	bash insertVBnameForNode.sh
	echo "add ambari server start to bootstrap.sh script"
	cat extended_bootstrap.txt >> ambari-vagrant/centos6.4/bootstrap.sh 
	echo "Do you wish to append cluster nodes to hosts file? should be done first time or else it will not work [Yes=1|No=2]"
	select yn in "Yes" "No"; do
	    case $yn in
		Yes )   echo "append the cluster nodes to your hosts file";
			sudo bash install_hosts.sh;
 			echo "nodes appended to /etc/hosts file";
			break;;
		No ) 	echo "nodes NOT appended to /etc/hosts file";
			break;;
	    esac
	done
# start vagrant to create a private key as ~/.vagrant.d/insecure_private_key	
	vagrant &>/dev/null 
fi

# starting 3 VMs
echo "starting 3 cluster nodes in VM"
cd ambari-vagrant
cd centos6.4
echo "Do you want to destroy previous VM's ? [Yes=1|No=2]"
select yn in "Yes" "No"; do
	case $yn in
		Yes )
			echo "destroy previous cluster";
			vagrant destroy -f;
			break;;
		No ) 	echo "no destruction of cluster";
                        break;;
	esac
done
echo "prepare insecure_private_key"
cp ~/.vagrant.d/insecure_private_key .
echo "start the nodes"
./up.sh 3
echo "Done."

