#
# http://mrbook.org/tutorials/make/
# http://www.cs.northwestern.edu/academics/courses/211/html/make.html
# http://www.cs.umd.edu/class/fall2002/cmsc214/Tutorial/makefile.html
#
#
CC=g++ -g -o bin/Debug/1_1_9_test 1_1_9_ProfileLog_test.cpp ../../1_1_Profile.hpp ../../md5.h ../../1_1_Profile.cpp ../../../../DataBaseControl/databasecontrol.hpp ../../../../DataBaseControl/databasecontrol.cpp ../../../../DataEncoderDecoder/DataEncoderDecoder/DataEncoder.h ../../../../DataEncoderDecoder/DataEncoderDecoder/DataEncoder.cpp ../../../../DataEncoderDecoder/DataEncoderDecoder/compression-lib/compression.h ../../../../DataEncoderDecoder/DataEncoderDecoder/compression-lib/compression.cpp ../../../../websocket_server/dataframe.cpp ../../../../websocket_server/dataframe_parser.cpp ../../../../websocket_server/reply.cpp ../../../../websocket_server/request_handler.cpp ../../../../websocket_server/request_parser.cpp ../../../../websocket_server/room.cpp ../../../../websocket_server/server.cpp ../../../../websocket_server/session.cpp ../../../../websocket_server/dataframe.hpp ../../../../websocket_server/dataframe_parser.hpp ../../../../websocket_server/header.hpp ../../../../websocket_server/participant.hpp ../../../../websocket_server/reply.hpp ../../../../websocket_server/request.hpp ../../../../websocket_server/request_handler.hpp ../../../../websocket_server/request_parser.hpp ../../../../websocket_server/room.hpp ../../../../websocket_server/server.hpp ../../../../websocket_server/session.hpp ../../../../websocket_server/wsclient/wsclient.hpp ../../../../websocket_server/wsclient/wsclient.cpp -L"/usr/local/lib/" -D__DEBUG__ -D__MSABI_LONG=long -lboost_system -lboost_signals -lboost_thread -lboost_filesystem -lpthread -lrt -std=gnu++11
CFLAGS= -Wno-unused-but-set-variable
TEST=./bin/Debug/1_1_9_test  
TESTFLAGS= --report_format=XML --report_level=detailed
TESTFLAGS2= --report_level=detailed
TESTFLAGS3= --report_format=XML --report_level=detailed
CONVERT=xsltproc -o test_results.html ../test_results.xslt 1_1_9_test_results.xml
CONVERT2=xsltproc -o test_results.txt ../test_results_text.xslt 1_1_9_test_results.xml

all:
	@ echo "------------------------"
	@ echo " Unittests "
	@ echo "------------------------"
	@ echo "ConnectToWebserver        "
	@ echo "CommunicateWithWebserver  "
	@ echo "LoginToProfile "
	@ echo "_1191_AddToLog      "
	@ echo "    "
	@ echo "------------------------"
	@ echo " ex. : make -f makefile_ubuntu.mak total -- will build and test ALL"
	@ echo " ex. : make -f makefile_ubuntu.mak test  -- will only run ALL test, not build"
	@ echo " ex. : make -f makefile_ubuntu.mak run unittest=<name> -- will run individual test - NOT build"
	@ echo " see output : make -f makefile_ubuntu.mak run unittest=\"_1191_AddToLog \&&\" "
	@ echo "------------------------"

total:	compile test final 	

compile:
	@ echo "------------------------"
	@ echo "Build started -- please wait..."	
	@ $(CC) $(CFLAGS)
	@ echo "Build ended --"
	@ echo "------------------------"

run: _run final
_run:
	@ echo " Test of DFD_1_1_9 "
	@ echo "-----------------------------------------------"
	@ $(TEST)$(TESTFLAGS) --log_level=test_suite --run_test=$(unittest)   > error.txt;$(CONVERT)
	@ $(TEST)$(TESTFLAGS3) --log_level=test_suite --run_test=$(unittest)   > error2.txt;$(CONVERT2)

test: _test final
_test:
	@ echo " Test of DFD_1_1_9 is started - please wait... " 
	@ echo "-----------------------------------------------" 
	@ $(TEST)$(TESTFLAGS) > error.txt;$(CONVERT) 
	@ $(TEST)$(TESTFLAGS3) > error2.txt;$(CONVERT2) 
	@ echo "-----------------------------------------------" > test_txt_result.txt 
	@ echo " Test of DFD_1_1_9  " >> test_txt_result.txt  
	@ echo "-----------------------------------------------" >> test_txt_result.txt 
final:
	@ cat test_results.txt | grep 'OK\|FAIL' >> test_txt_result.txt
	@ echo "--------------" >> test_txt_result.txt
	@ echo "Tests Complete" >> test_txt_result.txt
	@ echo "--------------" >> test_txt_result.txt
	@ printf "Passed: " >> test_txt_result.txt;(cat test_results.txt | grep -c 'OK'; printf "") >> test_txt_result.txt
	@ printf "Failed: " >> test_txt_result.txt;(cat test_results.txt | grep -c 'FAIL'; printf "") >> test_txt_result.txt
	@ echo "--------------" >> test_txt_result.txt
	@ cat test_txt_result.txt
	@ cat test_txt_result.txt > test_results.txt
	@ rm test_txt_result.txt
	@ rm error2.txt
preclean:
	@ rm error.txt 2>tmp
	@ rm test_results.txt 2>tmp
	@ rm test_results.html 2>tmp
	@ rm tmp
