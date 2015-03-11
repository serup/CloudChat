# 
# http://mrbook.org/tutorials/make/
#

CC=g++
CFLAGS=-c -D_WIN32 -D__MINGW32__ -D__CYGWIN__ -D__SunOS -std=gnu++11 
LDFLAGS=
SOURCES=dataencodertest.cpp ../../DataEncoder.cpp ../../compression-lib/compression.cpp
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=bin/Debug/dataencodertest
TEST=./bin/Debug/dataencodertest.exe 
TESTFLAGS= --report_format=XML --report_level=detailed
TESTFLAGS2= --report_level=detailed
TESTFLAGS3= --report_format=XML --report_level=detailed
CONVERT=xsltproc -o test_results.html ../test_results.xslt test_results.xml
CONVERT2=xsltproc -o test_results.txt ../test_results_text.xslt test_results.xml


all: $(SOURCES) $(EXECUTABLE) test clean
	
$(EXECUTABLE): $(OBJECTS) 
	@ $(CC) $(LDFLAGS) $(OBJECTS) -o $@

.cpp.o:
	@ echo "-----------------------------------------------" 
	@ echo " Build started - please wait..." 
	@ echo " Start building DataEncoderDecoder testsuite"
	@ echo "-----------------------------------------------" 
	@ $(CC) $(CFLAGS) $< -o $@
	@ echo "Finished building"
	@ echo "-----------------------------------------------"
testbasic:
	@ echo "-------------"
	@ echo "Tests Starting - testing DataEncoderDecoder"
	@ echo "-------------"
	@ $(TEST)$(TESTFLAGS) > error.txt;$(CONVERT) 
	@ $(TEST)$(TESTFLAGS2) > error.txt;
	@ cat test_results.xml 
	@ echo "-------------"
	@ echo Tests Complete
	@ echo "-------------"
test:
	@ echo " Test of DataEncoderDecoder - please wait... " 
	@ echo "-----------------------------------------------" 
	@ $(TEST)$(TESTFLAGS) > error.txt;$(CONVERT) 
	@ $(TEST)$(TESTFLAGS3) > error2.txt;$(CONVERT2) 
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
	@ cat test_txt_result.txt
	@ cat test_txt_result.txt > test_results.txt
	@ rm test_txt_result.txt
	@ rm error2.txt
clean:
	@ rm *.o

