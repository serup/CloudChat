BUILD := build
CONVERT=xsltproc -o test_results.html ./test/test_results.xslt ./test/test_results.xml
#
# NB! IF websocket server has failure or in some way does not disconnect then this script
# will NOT follow after the node test/index.js line !!!! node.js will hang this script - you can manually stop by killing 
# scanvaserver process from another terminal
#
# Run command line tests
# http://sc.tamu.edu/help/general/unix/redirection.html
# http://www.thegeekstuff.com/2011/10/grep-or-and-not-operators/
#
# how to fetch process id running task
# ps aux | grep jenkins | grep 1_1|grep 7788| awk '{print $2}'
# consider building a safe kill into this batch - pt. it will break if another thread is running
# one possible way
# kill -KILL $( ps aux | grep 1_1|grep 127. |awk '{print $2}')
# - however if process was not there, then this will also break batch - find some other way!
test:
	@ echo " ----------------------------------------------------------- "	
	@ echo "  Test of cloudchat is initializing - please wait... "	
	@ echo " ----------------------------------------------------------- "	
	@ test -f running.pid && rm running.pid
	@ test -f tmp && rm tmp 
	@ test -f tmp2 && rm tmp2 
	#@ (../../../codeblocks_projects/serup/websocket_server/bin/Debug/scanvaserver 0.0.0.0 7798 & echo $$! > running.pid ) > tmp & 
	#@ (cd ../../../codeblocks_projects/serup/DFDFunctions/1_1/bin/Debug && pwd && ./1_1 127.0.0.1 7798 & echo $$! >> running.pid) > tmp2 &
	@ (../../codeblocks_projects/websocket_server/bin/Debug/scanvaserver 0.0.0.0 7798 & echo $$! > running.pid ) > tmp & 
	@ (cd ../../codeblocks_projects/DFDFunctions/1_1/bin/Debug && pwd && ./1_1 127.0.0.1 7798 & echo $$! >> running.pid) > tmp2 &
	@ echo "var WebSocket = require('ws')" > test/result_index.js
	@ cat ../socketserverAPI/socketserverAPI.js >> test/result_index.js
	@ cat ../DataEncoderDecoder/dataencoderdecoder.js >> test/result_index.js
	@ cat ../DataEncoderDecoder/compression-lib/compression.js >> test/result_index.js
	@ cat test/index_cloudmanager.js >> test/result_index.js
	@ node test/result_index.js > error.txt 
	@ cat error.txt | grep "OK:\|FAIL:\|---\|Tests\|Passed\|Failed\|Expected\|Got\|Exception" > _error.txt
	@ echo "<TestSuite name=\"cloudchatTest\">" > test/test_results.xml
	@ cat error.txt | grep "TestCase" >> test/test_results.xml
	@ echo "</TestSuite>" >> test/test_results.xml
	@ cat _error.txt > error.txt
	@ rm _error.txt
	@ $(CONVERT) 2>&1
	@ cat error.txt
	@ test -f test/test_results.xml && rm test/test_results.xml
	@ echo "try to kill process "
	@ kill -KILL $$(cat running.pid)

.PHONY: test 
#
# node info:
# 
#
# Node is one of the easier projects to build. Just change the version as that continues to change.
#
# $ cd /usr/local/src
# $ sudo wget http://nodejs.org/dist/v0.8.21/node-v0.8.21.tar.gz
# $ sudo tar -xvzf node-v0.8.21.tar.gz
# $ cd node-v0.8.21
# $ sudo ./configure
# $ sudo make
# $ sudo make install
# $ which node
#
# You should see /usr/local/bin/node.
#
# REMEMBER to install WebSockets (ws)
# npm install ws
