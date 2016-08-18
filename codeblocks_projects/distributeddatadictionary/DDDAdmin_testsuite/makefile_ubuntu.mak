######################################################################
#                                                                    #
# Make file for DDDAdmin - Distributed Data Dictionary Administrator #
#                                                                    #
######################################################################
CC=g++ -g -o bin/Debug/DDDAdmin_test DDDAdmin_test.cpp ../datadictionarycontrol.hpp ../datadictionarycontrol.cpp -L"/usr/local/lib/" -D__DEBUG__ -D__MSABI_LONG=long -lboost_system -lboost_signals -lboost_thread -lboost_filesystem -lpthread -lrt -std=gnu++11
TEST=./bin/Debug/DDDAdmin_test
TESTFLAGS= --report_format=XML --report_level=detailed
TESTFLAGS2= --report_level=detailed
TESTFLAGS3= --report_format=XML --report_level=detailed
CONVERT=xsltproc -o test_results.html ../test_results.xslt test_results.xml
CONVERT2=xsltproc -o test_results.txt ../test_results_text.xslt test_results.xml


info:
	@ echo "------------------------------"
	@ echo " DDDAdmin test info "
	@ echo "------------------------------"
	@ echo "to build all : make -f makefile_ubuntu.mak total"
	@ echo "to test one unittest use following format: make -f makefile_ubuntu.mak unittest unittest=<name of unittest>"
	@ echo " "
	@ echo '**************************';echo 'UNIT TESTCASES :';echo '**************************';echo; fgrep BOOST_AUTO_TEST_CASE *.cpp | sed -e 's/BOOST_AUTO_TEST_CASE(//g' | sed -e 's/)//g'; echo '**************************';

all: compile test
total: compile test
list:
	@ echo '**************************';echo 'UNIT TESTCASES :';echo '**************************';echo; fgrep BOOST_AUTO_TEST_CASE *.cpp | sed -e 's/BOOST_AUTO_TEST_CASE(//g' | sed -e 's/)//g'; echo '**************************';


compile:
	@ echo "------------------------"
	@ echo "Build started -- please wait..."	
	@ $(CC)
	@ echo "Build ended --"
	@ echo "------------------------"
testbasic:
	$(TEST) $(TESTFLAGS) > error.txt;$(CONVERT)
test:
	@ echo " Test of DDDAdmin is started - please wait... " 
	@ echo "-----------------------------------------------" 
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
	@ cat test_txt_result.txt
	@ cat test_txt_result.txt > test_results.txt
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

.PHONY: info
