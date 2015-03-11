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
	@ echo "  Making Test of socketserverAPI is initializing - please wait... "	
	@ echo " ----------------------------------------------------------- "	
	@ make -f makefile_cygwin_ws.mak createtest > tmp
	@ echo "Test was created in file result_index.js"
	@ echo "To run the test - then write following:  make -f makefile_cygwin_ws.mak test"
	@ echo " ----------------------------------------------------------- "	

test:
	@ echo "  ********************************************************** "	
	@ echo "  Initializing socketserverAPI test - please wait... "	
	@ echo "  NB! if avast virus scan is on then it could make test fail "	
	@ echo "  ********************************************************** "	
	@ make -f makefile_cygwin_ws.mak servstart createtest runnode cleanup 
xtest:
	@ echo " ---------------------------------------------------------------------------------- "	
	@ echo "  Test of socketserverAPI with NO servers -- your own should run before this test  " > tmp2	
	@ echo " ---------------------------------------------------------------------------------- "	
	@ make -f makefile_cygwin_ws.mak runnode 
servstart:
	 #../../../codeblocks_projects/serup/websocket_server/bin/Debug/scanvaserver.exe 0.0.0.0 7788 > tmp &
	 #(cd ../../../codeblocks_projects/serup/DFDFunctions/1_1/bin/Debug && pwd && ./DFD_1_1.exe 127.0.0.1 7788) > tmp2 &
	 ../../codeblocks_projects/websocket_server/bin/Debug/scanvaserver.exe 0.0.0.0 7788 > tmp &
	 (cd ../../codeblocks_projects/DFDFunctions/1_1/bin/Debug && pwd && ./DFD_1_1.exe 127.0.0.1 7788) > tmp2 &
createtest:
	@ cat socketserverAPI.js > test/result_index.js
	@ cat ../DataEncoderDecoder/dataencoderdecoder.js >> test/result_index.js
	@ cat ../DataEncoderDecoder/compression-lib/compression.js >> test/result_index.js
	@ cat test/index.js >> test/result_index.js
runnode:
	@ echo "var WebSocket = require('ws')" > test/_result_index.js
	@ cat test/result_index.js >> test/_result_index.js
	@ node test/_result_index.js > error.txt 
	@ cat error.txt | grep "Test of\|OK:\|FAIL:\|---\|Tests\|Passed\|Failed\|Expected\|Got\|Exception" > _error.txt
	@ echo "<TestSuite name=\"socketserverAPITest\">" > test/test_results.xml
	@ cat error.txt | grep "TestCase" >> test/test_results.xml
	@ echo "</TestSuite>" >> test/test_results.xml
	@ cat tmp2 > detailed_error.txt
	@ cat _error.txt > error.txt
	@ rm _error.txt
	@ rm test/_result_index.js
	@ $(CONVERT) 2>&1
	@ cat error.txt
	@ rm test/test_results.xml
cleanup:
	@ taskkill /IM scanvaserver.exe /F
	@ taskkill /IM DFD_1_1.exe /F
	@ rm tmp
	@ rm tmp2

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

