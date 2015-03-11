#
# http://mrbook.org/tutorials/make/
# http://www.cs.northwestern.edu/academics/courses/211/html/make.html
# http://www.cs.umd.edu/class/fall2002/cmsc214/Tutorial/makefile.html
#
#
CC=g++ -g -o bin/Debug/ringbuffer_test ringbuffer_test.cpp ../../ringbuffer.hpp ../../../DataEncoderDecoder/DataEncoderDecoder/DataEncoder.cpp  ../../../DataEncoderDecoder/DataEncoderDecoder/DataEncoder.h ../../../DataEncoderDecoder/DataEncoderDecoder/compression-lib/compression.cpp ../../../DataEncoderDecoder/DataEncoderDecoder/compression-lib/compression.h -L"/usr/local/lib/" -D__MSABI_LONG=long -lboost_system -lboost_signals -lboost_thread -lpthread -lrt -std=gnu++11 
TEST=./bin/Debug/ringbuffer_test 
TESTFLAGS= --report_format=XML --report_level=detailed
TESTFLAGS2= --report_level=detailed
TESTFLAGS3= --report_format=XML --report_level=detailed
CONVERT=xsltproc -o test_results.html ../test_results.xslt test_results.xml
CONVERT2=xsltproc -o test_results.txt ../test_results_text.xslt test_results.xml

all: compile test

compile:
	@ echo "-----------------------------------------------" 
	@ echo " Build started - please wait..." 
	@ echo " Start building ringbuffer testsuite"
	@ echo "-----------------------------------------------" 
	@ $(CC)
	@ echo "Finished building"
	@ echo "-----------------------------------------------" 
testxml:
	$(TEST)$(TESTFLAGS) > error.txt;$(CONVERT) 
test:
	@ echo " Test of ringbuffer - please wait... " 
	@ echo "-----------------------------------------------" 
	@ $(TEST)$(TESTFLAGS) > error.txt;$(CONVERT) 
	@ $(TEST)$(TESTFLAGS3) > error2.txt;$(CONVERT2) 
	@ echo "-----------------------------------------------" > test_txt_result.txt 
	@ echo " Test of ringbuffer  " >> test_txt_result.txt  
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


#g++ -g -o bin/Debug/ringbuffer_test ringbuffer_test.cpp ../../ringbuffer.hpp  -L"/usr/local/lib/"  -D__MSABI_LONG=long -lboost_system -lboost_signals -lboost_thread -lpthread -lrt  -std=gnu++11
