#
# http://mrbook.org/tutorials/make/
# http://www.cs.northwestern.edu/academics/courses/211/html/make.html
# http://www.cs.umd.edu/class/fall2002/cmsc214/Tutorial/makefile.html
#
#
CC=g++ -g -o bin/Debug/webserver_test webserver_test.cpp ../../easywsclient/easywsclient.cpp ../../easywsclient/easywsclient.hpp ../../dataframe.hpp ../../dataframe.cpp ../../dataframe_parser.hpp ../../dataframe_parser.cpp ../../request_parser.hpp ../../request_parser.cpp ../../room.hpp ../../room.cpp ../../server.hpp ../../server.cpp ../../session.hpp ../../session.cpp ../../request_handler.hpp ../../request_handler.cpp ../../reply.hpp ../../reply.cpp ../../wsclient/wsclient.cpp ../../wsclient/wsclient.hpp ../../../DataEncoderDecoder/DataEncoderDecoder/compression-lib/compression.cpp ../../../DataEncoderDecoder/DataEncoderDecoder/DataEncoder.cpp -L"/usr/local/lib/" -D__MSABI_LONG=long -lboost_system -lboost_signals -lboost_thread -lpthread -lrt -std=gnu++11 
TEST=./bin/Debug/webserver_test 2>/dev/null 
TESTFLAGS= --report_format=XML --report_level=detailed
TESTFLAGS2= --report_level=detailed
TESTFLAGS3= --report_format=XML --report_level=detailed
CONVERT=xsltproc -o test_results.html ../test_results.xslt test_results.xml
CONVERT2=xsltproc -o test_results.txt ../test_results_text.xslt test_results.xml


all: compile test

compile:
	@ echo "-----------------------------------------------" 
	@ echo " Build started - please wait..." 
	@ echo " Start building websocketserver testsuite"
	@ echo "-----------------------------------------------" 
	@ $(CC) 2>tmp
	@ echo "Finished build"
	@ echo "-----------------------------------------------" 
testxml:
	@ $(TEST) $(TESTFLAGS) > error.txt;$(CONVERT) 
	@ cat error.txt
test:
	@ echo " Test of websocketserver - please wait... " 
	@ echo "-----------------------------------------------" 
	@ $(TEST) $(TESTFLAGS) > error.txt;$(CONVERT) 
	@ $(TEST) $(TESTFLAGS3) > error2.txt;$(CONVERT2) 
	@ echo "-----------------------------------------------" > test_txt_result.txt 
	@ echo " Test of websocketserver  " >> test_txt_result.txt  
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

