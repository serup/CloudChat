#
# http://mrbook.org/tutorials/make/
# http://www.cs.northwestern.edu/academics/courses/211/html/make.html
# http://www.cs.umd.edu/class/fall2002/cmsc214/Tutorial/makefile.html
#
#
CC=g++ -g -o bin/Debug/databasecontrol_test databasecontrol_test.cpp ../../databasecontrol.hpp ../../databasecontrol.cpp ../../../DataEncoderDecoder/DataEncoderDecoder/DataEncoder.cpp  ../../../DataEncoderDecoder/DataEncoderDecoder/DataEncoder.h ../../../DataEncoderDecoder/DataEncoderDecoder/compression-lib/compression.cpp ../../../DataEncoderDecoder/DataEncoderDecoder/compression-lib/compression.h -I"/cygdrive/c/cygwin/usr/include/w32api/" -L"/cygdrive/c/cygwin/usr/local/lib/" -DWIN32_LEAN_AND_MEAN -D__DEBUG__ -D_WIN32 -D_WIN32_WINNT -D__USE_W32_SOCKETS -D__CYGWIN__ -D__SunOS -D__MINGW32__ -D__MSABI_LONG=long -lws2_32 -lboost_system-mt.dll -lboost_signals.dll -lboost_thread.dll -lboost_filesystem.dll -std=gnu++11 
TEST=./bin/Debug/databasecontrol_test.exe
TESTFLAGS= --report_format=XML --report_level=detailed
TESTFLAGS2= --report_level=detailed
TESTFLAGS3= --report_format=XML --report_level=detailed
CONVERT=xsltproc -o test_results.html ../test_results.xslt test_results.xml
CONVERT2=xsltproc -o test_results.txt ../test_results_text.xslt test_results.xml


all: info	
info:
	@ echo "Use format : make unittest=<name of unittest>" 

test:
	@ ./bin/Debug/databasecontrol_test.exe > tmp --report_format=XML --report_level=detailed --log_level=test_suite --run_test=$(unittest);  xsltproc -o test_results.txt ../test_results_text.xslt test_results.xml;echo "*********************";echo "   Result"; echo "*********************"; cat test_results.txt | grep -e 'OK\|FAIL'; echo "*********************"; rm tmp
