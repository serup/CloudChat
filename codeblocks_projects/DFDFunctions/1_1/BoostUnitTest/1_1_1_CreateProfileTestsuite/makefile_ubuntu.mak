#
# http://mrbook.org/tutorials/make/
# http://www.cs.northwestern.edu/academics/courses/211/html/make.html
# http://www.cs.umd.edu/class/fall2002/cmsc214/Tutorial/makefile.html
#
#
CC=g++ -g -o bin/Debug/1_1_1_test 1_1_1_CreateProfile_test.cpp ../../1_1_Profile.hpp ../../md5.h ../../1_1_Profile.cpp ../../../../DataBaseControl/databasecontrol.hpp ../../../../DataBaseControl/databasecontrol.cpp ../../../../DataEncoderDecoder/DataEncoderDecoder/DataEncoder.h ../../../../DataEncoderDecoder/DataEncoderDecoder/DataEncoder.cpp ../../../../DataEncoderDecoder/DataEncoderDecoder/compression-lib/compression.h ../../../../DataEncoderDecoder/DataEncoderDecoder/compression-lib/compression.cpp ../../../../websocket_server/dataframe.cpp ../../../../websocket_server/dataframe_parser.cpp ../../../../websocket_server/reply.cpp ../../../../websocket_server/request_handler.cpp ../../../../websocket_server/request_parser.cpp ../../../../websocket_server/room.cpp ../../../../websocket_server/server.cpp ../../../../websocket_server/session.cpp ../../../../websocket_server/dataframe.hpp ../../../../websocket_server/dataframe_parser.hpp ../../../../websocket_server/header.hpp ../../../../websocket_server/participant.hpp ../../../../websocket_server/reply.hpp ../../../../websocket_server/request.hpp ../../../../websocket_server/request_handler.hpp ../../../../websocket_server/request_parser.hpp ../../../../websocket_server/room.hpp ../../../../websocket_server/server.hpp ../../../../websocket_server/session.hpp ../../../../websocket_server/wsclient/wsclient.hpp ../../../../websocket_server/wsclient/wsclient.cpp -L"/usr/local/lib/" -D__DEBUG__ -D__MSABI_LONG=long -lboost_system -lboost_signals -lboost_thread -lboost_filesystem -lpthread -lrt -std=gnu++11
TEST=./bin/Debug/1_1_1_test 
TESTFLAGS= --report_format=XML --report_level=detailed
TESTFLAGS2= --report_level=detailed
TESTFLAGS3= --report_format=XML --report_level=detailed
CONVERT=xsltproc -o test_results.html ../test_results.xslt 1_1_1_test_results.xml
CONVERT2=xsltproc -o test_results.txt ../test_results_text.xslt 1_1_1_test_results.xml

all: compile test

compile:
	@ echo "***********************************************"
	@ echo " Build started - please wait..." 
	@ echo " Start building 1_1_1_CreateProfileTestsuite"
	@ echo "***********************************************"
	@ $(CC)
	@ echo "Build ended"
	@ echo "***********************************************"

test:
	@ echo " Test of DFD_1_1_1 is started - please wait... " 
	@ echo "***********************************************"
	@ $(TEST)$(TESTFLAGS) > error.txt;$(CONVERT) 
	@ $(TEST)$(TESTFLAGS3) > error2.txt;$(CONVERT2) 
	@ echo "-----------------------------------------------" > test_txt_result.txt 
	@ echo " Test of DFD_1_1_1  " >> test_txt_result.txt  
	@ echo "-----------------------------------------------" >> test_txt_result.txt 
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

