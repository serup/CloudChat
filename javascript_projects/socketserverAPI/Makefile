BUILD := build
CONVERT=xsltproc -o test_results.html ./test/test_results.xslt ./test/test_results.xml

#
# Run command line tests
# http://sc.tamu.edu/help/general/unix/redirection.html
# http://www.thegeekstuff.com/2011/10/grep-or-and-not-operators/
test:
	#@ ../../../codeblocks_projects/serup/websocket_server/bin/Debug/scanvaserver.exe 0.0.0.0 7777 &
	@ ../../codeblocks_projects/websocket_server/bin/Debug/scanvaserver.exe 0.0.0.0 7777 &
	@ cat socketserverAPI.js > test/result_index.js
	@ cat ../DataEncoderDecoder/dataencoderdecoder.js >> test/result_index.js
	@ cat ../DataEncoderDecoder/compression-lib/compression.js >> test/result_index.js
	@ cat test/index.js >> test/result_index.js
	@ echo Test is now READY -- however
	@ echo THIS cygwin makefile can NOT run node.js with websockets -- use index.html test file with browser instead
	@ echo REMEMBER to KILL scanvaserver process when done - this should happen after you press a key 
	@ read -p "Press any key to continue... Server will be stopped!! " -n1 -s
	@ echo " "
	@ taskkill /IM scanvaserver.exe /F
.PHONY: test 
