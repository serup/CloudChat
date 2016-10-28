#
# NB!REMEMBER SPACE BETWEEN COMMANDS IN UBUNTU
# EX.  $(CC)$(CFLAGS)  WOULD WORK IN CYGWIN BUT IN UBUNTU THIS IS NEEDED $(CC) $(CFLAGS)
#

CCall=g++ -g -o bin/Debug/dataencodertest  dataencodertest.cpp ../../DataEncoder.cpp ../../compression-lib/compression.cpp -L"/usr/local/lib/" -lboost_regex -D__DEBUG__ -D__MSABI_LONG=long -DBOOST_LOG_DYN_LINK -lboost_system -lboost_log -lboost_signals -lboost_thread -lboost_filesystem -lpthread -lrt -std=gnu++11

TEST=./bin/Debug/dataencodertest
TESTFLAGS= --report_format=XML --report_level=detailed
TESTFLAGS2= --report_level=detailed
TESTFLAGS3= --report_format=XML --report_level=detailed
CONVERT=xsltproc -o test_results.html ../test_results.xslt test_results.xml
CONVERT2=xsltproc -o test_results.txt ../test_results_text.xslt test_results.xml

all: buildstartmsg compile buildfinishmsg display_waitfortest test

total: compile display_waitfortest test
_total: compile _test


buildstartmsg:
	@ echo "-----------------------------------------------" 
	@ echo " Build started - please wait..." 
	@ echo " Start building DataEncoderDecoder testsuite"
	@ echo "-----------------------------------------------" 
buildfinishmsg:
	@ echo "Finished building"
	@ echo "-----------------------------------------------"

.cpp.o:
	@ $(CC) $(CFLAGS) $< -o $@ 

compile:
	@ $(CCall) 

nooutputcompile:
	@ $(CC) $(CFLAGS) $< -o $@ 2>err.log

testbasic:
	$(TEST) $(TESTFLAGS) > error.txt;$(CONVERT)
display_waitfortest:
	@ echo " Test of DataEncoderDecoder - please wait... " 
	@ echo "-----------------------------------------------" 

_test:
	@ $(TEST) $(TESTFLAGS) > error.txt;$(CONVERT) 
	@ $(TEST) $(TESTFLAGS3) > error2.txt;$(CONVERT2) 
	@ echo "-----------------------------------------------" > test_txt_result.txt 
	@ echo " Test of DataEncoderDecoder  " >> test_txt_result.txt  
	@ echo "-----------------------------------------------" >> test_txt_result.txt 
	@ cat test_results.txt | grep 'OK\|FAIL' >> test_txt_result.txt
	@ echo "--------------" >> test_txt_result.txt
	@ echo "Tests Complete" >> test_txt_result.txt
	@ echo "--------------" >> test_txt_result.txt
	@ printf "Passed: " >> test_txt_result.txt;(cat test_results.txt | grep -c 'OK'; printf "") >> test_txt_result.txt
	@ printf "Failed: " >> test_txt_result.txt;(cat test_results.txt | grep -c 'FAIL'; printf "") >> test_txt_result.txt 
	@ echo "--------------" >> test_txt_result.txt
	@ cat test_txt_result.txt > test_results.txt
	@ cat test_results.txt
	-@ rm test_txt_result.txt
	-@ rm error2.txt

test:
	@ $(TEST) $(TESTFLAGS) > error.txt;$(CONVERT) 
	@ $(TEST) $(TESTFLAGS3) > error2.txt;$(CONVERT2) 
	@ echo "-----------------------------------------------" > test_txt_result.txt 
	@ echo " Test of DataEncoderDecoder  " >> test_txt_result.txt  
	@ echo "-----------------------------------------------" >> test_txt_result.txt 
	@ cat test_results.txt | grep 'OK\|FAIL' >> test_txt_result.txt
	@ echo "--------------" >> test_txt_result.txt
	@ echo "Tests Complete" >> test_txt_result.txt
	@ echo "--------------" >> test_txt_result.txt
	@ printf "Passed: " >> test_txt_result.txt;(cat test_results.txt | grep -c 'OK'; printf "") >> test_txt_result.txt
	@ printf "Failed: " >> test_txt_result.txt;(cat test_results.txt | grep -c 'FAIL'; printf "") >> test_txt_result.txt 
	@ echo "--------------" >> test_txt_result.txt
	@ echo "errors if any : " >> test_txt_result.txt
	-@ cat error.txt >> test_txt_result.txt
	@ cat test_txt_result.txt
	@ cat test_txt_result.txt > test_results.txt
	-@ rm test_txt_result.txt
	-@ rm error2.txt
clean:
	-@ rm *.gcda
	-@ rm *.gcno
	-@ rm *.o

