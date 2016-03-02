#!/bin/bash

#TODO: based on following make a script which can insert a line before a line in a file

for c in `seq -f '%02g' 10`;
do
	i=10#$c  # change from octal, since 0 infront signals it is octal, to base 10 inorder to calculate 
     	tmp1="sed -n 'H;\${x;s/^\n//;s/c64${c}.vm.network .*\n/c64${c}.vm.provider :virtualbox do |vb| vb.name=\"c64${c}\" end\n    &/;p;}' newVagrantfile$((i-1)) > newVagrantfile$((i))";
	echo "$tmp1" >> tmpsubst.sh
done
	
