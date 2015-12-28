#!/bin/sh
# should remove files of older iteration
# example:
# <filename>_<iterationnumber>.jpg
# ABC_8.jpg
# ABC_7.jpg - remove
# ABC_6.jpg - remove
#
# usage : rmyoung.sh ABC_??.jpg 
#
# 
filename=$1
prefix="${filename%%??.*}"
echo "prefix: $prefix"
suffix="${filename##*.}"
echo "suffix: $suffix"
echo found files :
ls -1 ${prefix}??.*
echo now remove all except last filename
lastfile=$(ls -1 ${prefix}??.* | sort -V | tail -1)
echo "lastfile = ${lastfile}"
#echo remove following :
#ls -1 ${prefix}??.* | grep -v ${lastfile}
count="$(ls -1 ${prefix}??.* | grep -v ${lastfile}| wc -l)"
#echo "count = ${count}"
if [ $count -gt 0 ]  
then
  ls -1 ${prefix}??.* | grep -v ${lastfile}| xargs rm 
else
  echo no files to remove
fi
