CC=g++
CFLAGS= -c -lboost_system -std=c++11 --coverage
LDFLAGS= --coverage
SOURCES=compressionTest.cpp ../compression.h ../compression.cpp 
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=bin/Debug/compressiontest
TEST=./bin/Debug/compressiontest
TESTFLAGS= --report_format=XML --report_level=detailed
CONVERT=xsltproc -o test_results.html ../test_results.xslt test_results.xml
CONVERT2=xsltproc -o test_results.txt ../test_results_text.xslt test_results.xml

all: $(SOURCES) $(EXECUTABLE) clean test

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@


test:
	@ echo " Test of compression-lib is started - please wait... "
	@ echo "***********************************************"
	@ $(TEST) $(TESTFLAGS) > error.txt;$(CONVERT)
	@ $(TEST) $(TESTFLAGS) > error.txt;$(CONVERT2)
	@ echo "-----------------------------------------------" > test_txt_result.txt
	@ echo " Test of compression-lib  " >> test_txt_result.txt
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

clean:
	rm *.o
