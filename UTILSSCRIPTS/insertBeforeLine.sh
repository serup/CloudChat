#!/bin/bash

#TODO: based on following make a script which can insert a line before a line in a file

for c in `seq -f '%02g' 10`;
do
	i=10#$c  # change from octal, since 0 infront signals it is octal, to base 10 inorder to calculate 
     	tmp1="sed -n 'H;\${x;s/^\n//;s/c64${c}.vm.network .*\n/c64${c}.vm.provider :virtualbox do |vb| vb.name=\"c64${c}\" end\n    &/;p;}' newVagrantfile$((i-1)) > newVagrantfile$((i))";
	echo "$tmp1" >> tmpsubst.sh
done

# info - example :
# 
# example file test.txt :
# 
# hello line 1
# hello line 2
# hello line 3
# 
# script:
# 
# sed -n 'H;${x;s/^\n//;s/hello line 2/hello new line\n&/;p;}' test.txt > test.txt.2
# 
# output file test.txt.2
# 
# hello line 1
# hello new line
# hello line 2
# hello line 3
# 
# NB! notice that the sed has as beginning a substitution of a newline to no space - this is necessary otherwise resulting file will have one empty line in the beginning
# 
# The script finds the line containing "hello line 2", it then inserts a new line above -- "hello new line"
# 
# explanation of sed commands:
# 
# sed -n:
# suppress automatic printing of pattern space
# 
# H;${x;s/test/next/;p}
# 
# /<pattern>/  search for a <pattern>
# ${}  do this 'block' of code
# H    put the pattern match in the hold space
# s/   substitute test for next everywhere in the space
# x    swap the hold with the pattern space
# p    Print the current pattern hold space. 
