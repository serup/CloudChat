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

all:
	@ echo " ----------------------------------------------------------- "	
	@ echo "  Test of CloudChat is initializing - please wait... "	
	@ echo " ----------------------------------------------------------- "	
	@ make -f makefile_cygwin_ws.mak test
test:
	@ ../../../codeblocks_projects/serup/websocket_server/bin/Debug/scanvaserver.exe 0.0.0.0 7798 > tmp &
	@ echo "var WebSocket = require('ws')" > test/result_index.js
	@ cat ../socketserverAPI/socketserverAPI.js >> test/result_index.js
	@ cat ../DataEncoderDecoder/dataencoderdecoder.js >> test/result_index.js
	@ cat ../DataEncoderDecoder/compression-lib/compression.js >> test/result_index.js
	@ cat test/index_cloudmanager.js >> test/result_index.js
	@ node test/result_index.js > error.txt 
	@ cat error.txt | grep "Test of\|OK:\|FAIL:\|---\|Tests\|Passed\|Failed\|Expected\|Got\|Exception" > _error.txt
	@ echo "<TestSuite name=\"cloudchatTest\">" > test/test_results.xml
	@ cat error.txt | grep "TestCase" >> test/test_results.xml
	@ echo "</TestSuite>" >> test/test_results.xml
	@ cat _error.txt > error.txt
	@ rm _error.txt
	@ $(CONVERT) 2>&1
	@ cat error.txt
	@ rm test/test_results.xml
	@ taskkill /IM scanvaserver.exe /F

.PHONY: test 
#
# node and websocket info:
# 
# ONLY INSTALL NODE WITH WINDOWS INSTALLER, THEN COPY ALL TO CYGWIN /usr/local/bin
# afterwards use following to install ws with npm
#
# node "C:\cygwin\usr\local\bin\node_modules\npm\bin\npm-cli.js" install -g ws
#
# Important the folder node_modules with ws and websockets folders inside – must reside in home folder
# Ex.:
# cygwin/home/serup/node_modules

