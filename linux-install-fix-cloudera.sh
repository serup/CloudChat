#!/usr/bin/env bash
echo "fix no such file or directory cdh3 cdh4"
mkdir -p /tmp/cloudera/key
echo "Download cdh4 key"
wget  http://archive.cloudera.com/cdh4/ubuntu/lucid/amd64/cdh/archive.key -O /tmp/cloudera/key/archive.key
echo "add cloudera key"
sudo apt-key add /tmp/cloudera/key/archive.key
echo "structure : "
find  /usr/share/doc/c* -maxdepth 1  -type d|grep cdh4
echo "Create new folder structure (/usr/share/doc/cdh3-repository)"
sudo mkdir -p /usr/share/doc/cdh3-repository
echo "Copy files from /usr/share/doc/cdh3-repository unto /usr/share/doc/cdh4-repository"
sudo cp --recursive /usr/share/doc/cdh4-repository/* /usr/share/doc/cdh3-repository/
echo "structure:"
find  /usr/share/doc/cdh* -maxdepth 1  -type d
echo "do fix "
sudo apt-get install -f
