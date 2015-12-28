#
# http://mrbook.org/tutorials/make/
# http://www.cs.northwestern.edu/academics/courses/211/html/make.html
# http://www.cs.umd.edu/class/fall2002/cmsc214/Tutorial/makefile.html
#
#
CC=g++ -g -o bin/Debug/databasecontrol_test databasecontrol_test.cpp ../../databasecontrol.hpp ../../databasecontrol.cpp ../../../DataEncoderDecoder/DataEncoderDecoder/DataEncoder.cpp  ../../../DataEncoderDecoder/DataEncoderDecoder/DataEncoder.h ../../../DataEncoderDecoder/DataEncoderDecoder/compression-lib/compression.cpp ../../../DataEncoderDecoder/DataEncoderDecoder/compression-lib/compression.h -L"/usr/local/lib/" -D__MSABI_LONG=long -lboost_system -lboost_signals -lboost_thread -lpthread -lrt -lboost_filesystem -std=gnu++11 
CFLAGS= --coverage  -Wno-unused-but-set-variable
TEST=./bin/Debug/databasecontrol_test 
TESTFLAGS= --report_format=XML --report_level=detailed
TESTFLAGS2= --report_level=detailed
TESTFLAGS3= --report_format=XML --report_level=detailed
CONVERT=xsltproc -o test_results.html ../test_results.xslt test_results.xml
CONVERT2=xsltproc -o test_results.txt ../test_results_text.xslt test_results.xml

all: compile test

compile:
	@ echo "-----------------------------------------------" 
	@ echo " Build started - please wait..." 
	@ echo " Start building databasecontrol testsuite"
	@ echo "-----------------------------------------------" 
	@ $(CC) $(CFLAGS)
	@ echo "Finished building"
	@ echo "-----------------------------------------------" 
testxml:
	$(TEST)$(TESTFLAGS) > error.txt;$(CONVERT) 
test:
	@ echo " Test of databasecontrol - please wait... " 
	@ echo "-----------------------------------------------" 
	@ $(TEST)$(TESTFLAGS) > error.txt;$(CONVERT) 
	@ $(TEST)$(TESTFLAGS3) > error2.txt;$(CONVERT2) 
	@ echo "-----------------------------------------------" > test_txt_result.txt 
	@ echo " Test of databasecontrol  " >> test_txt_result.txt  
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


#g++ -g -o bin/Debug/databasecontrol_test databasecontrol_test.cpp ../../databasecontrol.hpp  -L"/usr/local/lib/"  -D__MSABI_LONG=long -lboost_system -lboost_signals -lboost_thread -lpthread -lrt  -std=gnu++11
