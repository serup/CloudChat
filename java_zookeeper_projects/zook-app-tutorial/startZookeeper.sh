#!/usr/bin/env bash
echo "This will start the zookeeper server on localhost"
echo "-read more info here : http://tutorialforlinux.com/2014/10/10/zookeeper-quick-start-on-ubuntu-14-04-trusty-lts-32-64bit-linux-easy-guide/"
echo "* starting zookeeper"
/opt/zookeeper-3.4.8/bin/zkServer.sh start
echo ".Done"
