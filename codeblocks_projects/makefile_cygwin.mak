#
# http://mrbook.org/tutorials/make/
# http://www.cs.northwestern.edu/academics/courses/211/html/make.html
# http://www.cs.umd.edu/class/fall2002/cmsc214/Tutorial/makefile.html
#
# IMPORTANT! HAVE LINKS AFTER THE OBJECTS FILES, LINK ORDER MATTERS
# TODO: try to make this makefile fetch obj files from obj folder, thus making
# it compatible with the C::B project build - sofar I did not succeed in this

 # delete duplicate, nonconsecutive lines from a file. Beware not to
 # overflow the buffer size of the hold space, or else use GNU sed.
 #sed -n 'G; s/\n/&&/; /^\([ -~]*\n\).*\n\1/d; s/\n//; h; P'
 
 # delete duplicate, consecutive lines from a file (emulates "uniq").
 # First line in a set of duplicate lines is kept, rest are deleted.
 #sed '$!N; /^\(.*\)\n\1$/!P; D'


all:
	@ echo "------------------------"
	@ echo " Build options..."
	@ echo "------------------------"
	@ echo "dataencoder     -- builds the dataencoderdecoder project"
	@ echo "database        -- builds the databasecontrol project"
	@ echo "websocketserver -- builds the scanvaserver"
	@ echo "ringbuf         -- builds the Ringbuffer project"
	@ echo "dfdfunc         -- builds DFDFunctions projects"
	@ echo "req             -- builds requesthandler project"
	@ echo "total           -- builds ALL projects"
	@ echo "------------------------"
	@ echo " ex. : make -f makefile_cygwin.mak total"
	@ echo " ex. : make -f makefile_cygwin.mak testtotal"
	@ echo "------------------------"

total:	msgbegin dataencoder2 database2 websocketserver2 ringbuf2 dfdfunc2 dfdfunc3 dfdfunc4 dfdfunc5 req6 msgend 	
testtotal:	msgbegin dataencoder3 database3 websocketserver3 ringbuf3 dfdfunc7 dfdfunc8 dfdfunc9 req7 msgend 	

msgbegin:
	@ echo "------------------------"
	@ echo "Build started -- please wait..."	
	@ echo "------------------------"
dataencoder: msgbegin dataencoder2 msgend	
dataencoder2:
	@ cd DataEncoderDecoder/DataEncoderDecoder/BoostUnitTest/DataEncoderTestSuite;\
	make -f makefile_cygwin.mak > ../../../../output.txt
dataencoder3:
	@ cd DataEncoderDecoder/DataEncoderDecoder/BoostUnitTest/DataEncoderTestSuite;\
	make -f makefile_cygwin.mak test > ../../../../output.txt
database: msgbegin database2 msgend
database2:
	@ cd DataBaseControl/BoostUnitTest/databasecontrol_testsuite;\
	make -f makefile_cygwin.mak >> ../../../output.txt
database3:
	@ cd DataBaseControl/BoostUnitTest/databasecontrol_testsuite;\
	make -f makefile_cygwin.mak test >> ../../../output.txt
websocketserver: msgbegin websocketserver2 msgend
websocketserver2:
	@ cd websocket_server;\
	make -f makefile_webserver_cygwin.mak >> ../output.txt
	@ cd websocket_server/BoostUnitTest/websocketserver_testsuite;\
	make -f makefile_cygwin.mak >> ../../../output.txt
websocketserver3:
	@ cd websocket_server/BoostUnitTest/websocketserver_testsuite;\
	make -f makefile_cygwin.mak test >> ../../../output.txt
ringbuf: msgbegin ringbuf2 msgend
ringbuf2:
	@ cd Ringbuffer/BoostUnitTest/ringbuffer_testsuite;\
	make -f makefile_cygwin.mak >> ../../../output.txt
ringbuf3:
	@ cd Ringbuffer/BoostUnitTest/ringbuffer_testsuite;\
	make -f makefile_cygwin.mak test >> ../../../output.txt
dfdfunc: msgbegin dfdfunc2 dfdfunc3 dfdfunc4 dfdfunc5 msgend
dfdfunc2:
	@ cd DFDFunctions/1_1;\
	make -f makefile_cygwin.mak >> ../../output.txt
dfdfunc3:
	@ cd DFDFunctions/1_1/BoostUnitTest/1_1_1_CreateProfileTestsuite;\
	make -f makefile_cygwin.mak >> ../../../../output.txt
dfdfunc4:
	@ cd DFDFunctions/1_1/BoostUnitTest/1_1_6_LoginProfileTestsuite;\
	make -f makefile_cygwin.mak total >> ../../../../output.txt
dfdfunc5:
	@ cd DFDFunctions/1_1/BoostUnitTest/1_1_9_ProfileLogTestsuite;\
	make -f makefile_cygwin.mak total >> ../../../../output.txt
dfdfunc7:
	@ cd DFDFunctions/1_1/BoostUnitTest/1_1_1_CreateProfileTestsuite;\
	make -f makefile_cygwin.mak test >> ../../../../output.txt
dfdfunc8:
	@ cd DFDFunctions/1_1/BoostUnitTest/1_1_6_LoginProfileTestsuite;\
	make -f makefile_cygwin.mak test >> ../../../../output.txt
dfdfunc9:
	@ cd DFDFunctions/1_1/BoostUnitTest/1_1_9_ProfileLogTestsuite;\
	make -f makefile_cygwin.mak test >> ../../../../output.txt
req: msgbegin req6 msgend
req6:
	@ cd websocket_server/BoostUnitTest/request_handler_testsuite;\
	make -f makefile_cygwin.mak >> ../../../output.txt
req7:
	@ cd websocket_server/BoostUnitTest/request_handler_testsuite;\
	make -f makefile_cygwin.mak test >> ../../../output.txt
msgend:
	@ echo "Build ended --"
	@ echo "------------------------"
	@ (cat output.txt | grep 'Failed\|Passed\|---\|Test of\|OK\|FAIL' > result.txt; cat result.txt; (cat result.txt|grep -c 'OK';printf "")>pass.txt; (cat result.txt|grep -c 'FAIL';printf "")>fail.txt; echo Total; printf "Passed: ";cat pass.txt; printf "Failed: "; cat fail.txt ; echo ---------)>_total_result.txt; rm result.txt; rm fail.txt; rm pass.txt ; rm output.txt
	@ cat _total_result.txt | sed '$$!N; /^\(.*\)\n\1$$/!P; D' > total_result.txt
	@ rm _total_result.txt
	@ cat total_result.txt

.PHONY: info 
