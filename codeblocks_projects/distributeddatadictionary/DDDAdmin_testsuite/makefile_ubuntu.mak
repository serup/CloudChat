######################################################################
#                                                                    #
# Make file for DDDAdmin - Distributed Data Dictionary Administrator #
#                                                                    #
######################################################################
CC=g++ -g -o bin/Debug/DDDAdmin_test DDDAdmin_test.cpp ../datadictionarycontrol.hpp ../datadictionarycontrol.cpp ../md5.h ../../DataEncoderDecoder/DataEncoderDecoder/DataEncoder.h ../../DataEncoderDecoder/DataEncoderDecoder/DataEncoder.cpp ../../DataEncoderDecoder/DataEncoderDecoder/compression-lib/compression.h ../../DataEncoderDecoder/DataEncoderDecoder/compression-lib/compression.cpp -L"/usr/local/lib/" -D__DEBUG__ -D__MSABI_LONG=long -DBOOST_LOG_DYN_LINK -lboost_system -lboost_log -lboost_signals -lboost_thread -lboost_filesystem -lpthread -lrt -std=gnu++11
TEST=./bin/Debug/DDDAdmin_test
TESTFLAGS= --report_format=XML --report_level=detailed
TESTFLAGS2= --report_level=detailed
TESTFLAGS3= --report_format=XML --report_level=detailed
CONVERT=xsltproc -o test_results.html ../test_results.xslt test_results.xml
CONVERT2=xsltproc -o test_results.txt ../test_results_text.xslt test_results.xml
XMLRESULT=xmllint --format --recover xmlresult.xml  > xmlresult.formatted.xml
XMLRESULT2=xmllint --format --recover xmlresult2.xml  > xmlresult2.formatted.xml
XMLRESULT3=xmllint --format --recover xmlresult3.xml  > xmlresult3.formatted.xml

info:
	@ echo "------------------------------"
	@ echo " DDDAdmin test info "
	@ echo "------------------------------"
	@ echo "to build all (show result without detail) : make -f makefile_ubuntu.mak all"
	@ echo "to build all (show detailed result): make -f makefile_ubuntu.mak total"
	@ echo "to show result : make -f makefile_ubuntu.mak result"
	@ echo ""
	@ echo "to test one unittest use following format: make -f makefile_ubuntu.mak unittest unittest=<name of unittest>"
	@ echo "to show BFi xml test result in color format in VIM use following: make -f makefile_ubuntu.mak xml"
	@ echo " "
	@ echo '**************************';echo 'UNIT TESTCASES :';echo '**************************';echo; fgrep BOOST_AUTO_TEST_CASE *.cpp | sed -e 's/BOOST_AUTO_TEST_CASE(//g' | sed -e 's/)//g'; echo ''; echo '**************************';

all: compile test_without_detail
total: compile display_waitfortest test
_total: nooutputcompile test
list:
	@ echo '**************************';echo 'UNIT TESTCASES :';echo '**************************';echo; fgrep BOOST_AUTO_TEST_CASE *.cpp | sed -e 's/BOOST_AUTO_TEST_CASE(//g' | sed -e 's/)//g'; echo '**************************';

nooutputcompile:
	@ $(CC)
compile:
	@ echo "------------------------"
	@ echo "Build started -- please wait..."	
	@ mkdir -p bin/Debug
	@ $(CC)
	@ echo "Build ended --"
	@ echo "------------------------"
testbasic:
	$(TEST) $(TESTFLAGS) > error.txt;$(CONVERT)
testbasictxt:
	$(TEST) $(TESTFLAGS3) > error22.txt;$(CONVERT2) 

display_waitfortest:
	@ echo " Test of DDDAdmin is started - please wait... " 
	@ echo "-----------------------------------------------" 

test_without_detail:
	@ $(TEST) $(TESTFLAGS) > error.txt;$(CONVERT) 
	@ $(TEST) $(TESTFLAGS3) > error2.txt;$(CONVERT2) 
	@ echo "-----------------------------------------------" > test_txt_result.txt 
	@ echo " Test of DDDAdmin  " >> test_txt_result.txt  
	@ echo "-----------------------------------------------" >> test_txt_result.txt 
	@ cat test_results.txt | grep 'OK\|FAIL' >> test_txt_result.txt
	@ echo "--------------" >> test_txt_result.txt
	@ echo "Tests Complete" >> test_txt_result.txt
	@ echo "--------------" >> test_txt_result.txt
	@ printf "Passed: " >> test_txt_result.txt;(cat test_results.txt | grep -c 'OK'; printf "") >> test_txt_result.txt
	@ printf "Failed: " >> test_txt_result.txt;(cat test_results.txt | grep -c 'FAIL'; printf "") >> test_txt_result.txt
	@ echo "--------------" >> test_txt_result.txt
	@ cat test_txt_result.txt > test_results.txt
	@ echo "-----------------------------------------------" 
	@ ./result.sh|tail -n +2
	@ rm test_txt_result.txt
	@ rm error2.txt
result:
	@ echo "-----------------------------------------------" 
	@ ./result.sh|tail -n +2
test:

	@ $(TEST) $(TESTFLAGS3) > error2.txt;$(CONVERT2) 
	@ echo "-----------------------------------------------" > test_txt_result.txt 
	@ echo " Test of DDDAdmin  " >> test_txt_result.txt  
	@ echo "-----------------------------------------------" >> test_txt_result.txt 
	@ cat test_results.txt | grep 'OK\|FAIL' >> test_txt_result.txt
	@ echo "--------------" >> test_txt_result.txt
	@ echo "Tests Complete" >> test_txt_result.txt
	@ echo "--------------" >> test_txt_result.txt
	@ printf "Passed: " >> test_txt_result.txt;(cat test_results.txt | grep -c 'OK'; printf "") >> test_txt_result.txt
	@ printf "Failed: " >> test_txt_result.txt;(cat test_results.txt | grep -c 'FAIL'; printf "") >> test_txt_result.txt
	@ echo "--------------" >> test_txt_result.txt
	@ cat test_txt_result.txt > test_results.txt
	@ echo " " >> test_results.txt
	@ echo "- list of errors if any" >> test_results.txt
	@ cat error.txt >> test_results.txt
	@ cat test_results.txt
	@ rm test_txt_result.txt
	@ rm error2.txt
unittest:
	@ $(TEST) $(TESTFLAGS) --log_level=test_suite --run_test=$(unittest) > error.txt;$(CONVERT)
	@ $(TEST) $(TESTFLAGS3) --log_level=test_suite --run_test=$(unittest) > error2.txt;$(CONVERT2) 
	@ echo "-----------------------------------------------" > test_txt_result.txt 
	@ echo " Unit Test of DDDAdmin  " >> test_txt_result.txt  
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
	@ rm output*.txt
xml:
	@ $(XMLRESULT)
	@ vim xmlresult.formatted.xml


xml2:
	@ $(XMLRESULT2)
	@ vim xmlresult2.formatted.xml

xml3:
	@ $(XMLRESULT3)
	@ vim xmlresult3.formatted.xml


.PHONY: info
