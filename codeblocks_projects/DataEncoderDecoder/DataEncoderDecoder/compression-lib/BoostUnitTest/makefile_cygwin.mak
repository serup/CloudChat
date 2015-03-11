#
# http://mrbook.org/tutorials/make/
# http://www.cs.northwestern.edu/academics/courses/211/html/make.html
# http://www.cs.umd.edu/class/fall2002/cmsc214/Tutorial/makefile.html
#
#
CC=g++ -g -o bin/Debug/compressiontest compressionTest.cpp ../compression.h ../compression.cpp  -L"/cygdrive/c/cygwin/usr/local/lib/" -DWIN32_LEAN_AND_MEAN -D__DEBUG__ -D_WIN32 -D_WIN32_WINNT -D__USE_W32_SOCKETS -D__CYGWIN__ -D__SunOS -D__MINGW32__ -D__MSABI_LONG=long -lws2_32 -lboost_system.dll -lboost_signals.dll -lboost_thread.dll -std=gnu++11 
TEST=./bin/Debug/compressiontest.exe 
TESTFLAGS= --report_format=XML --report_level=detailed
CONVERT=xsltproc -o test_results.html ../test_results.xslt test_results.xml 
CONVERT2=xsltproc -o test_results.txt ../test_results_text.xslt test_results.xml

all: compile test

compile:
	@ echo "Build started - please wait, could take a minute"
	@ $(CC)
	@ echo "Build finished"
	
test:
	@ $(TEST)$(TESTFLAGS) > error.txt; $(CONVERT2) 
	@ $(TEST)$(TESTFLAGS) > error2.txt; $(CONVERT) 2>&1; 
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
	@ rm error2.txt
clean:
	@ rm *.o

