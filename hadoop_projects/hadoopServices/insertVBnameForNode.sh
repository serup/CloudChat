#!/bin/bash
#sed -n 'H;${x;s/^\n//;s/c6401.vm.network .*\n/c6401.vm.provider :virtualbox do |vb| vb.name="c6401" end\n    &/;p;}' Vagrantfile
echo "change to ambari-vagrant/centos6.4"
cd ambari-vagrant/centos6.4

cp Vagrantfile prevVagrantfile
cp Vagrantfile newVagrantfile0
for ((i=1; i < 11; i++))
do
	if  ((i < 10)) ; then 
        	tmp1="sed -n 'H;\${x;s/^\n//;s/c640${i}.vm.network .*\n/c640${i}.vm.provider :virtualbox do |vb| vb.name=\"c640${i}\" end\n    &/;p;}' newVagrantfile$((i-1)) > newVagrantfile${i}";
	else
        	tmp1="sed -n 'H;\${x;s/^\n//;s/c64${i}.vm.network .*\n/c64${i}.vm.provider :virtualbox do |vb| vb.name=\"c64${i}\" end\n    &/;p;}' newVagrantfile$((i-1)) > newVagrantfile${i}";
        fi 
	echo "$tmp1" >> tmpsubst.sh
done
echo "inserting vb.name(s)"
#cat tmpsubst.sh
bash tmpsubst.sh
rm tmpsubst.sh
mv Vagrantfile oldVagrantfile
mv newVagrantfile10 Vagrantfile
rm newVagrantfile*
echo "done - Vagrantfile in ambari-vagrant/centos6.4 is updated - old version is in prevVagrantfile - you can delete it if result is ok"
echo "change back to prev directory"
cd -
