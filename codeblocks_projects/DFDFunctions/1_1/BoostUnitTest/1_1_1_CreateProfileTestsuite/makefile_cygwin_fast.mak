#
# http://mrbook.org/tutorials/make/
# http://www.cs.northwestern.edu/academics/courses/211/html/make.html
# http://www.cs.umd.edu/class/fall2002/cmsc214/Tutorial/makefile.html
#
# IMPORTANT! HAVE LINKS AFTER THE OBJECTS FILES, LINK ORDER MATTERS
# TODO: try to make this makefile fetch obj files from obj folder, thus making
# it compatible with the C::B project build - sofar I did not succeed in this
CC=g++ -g -L"/cygdrive/c/cygwin/usr/local/lib/" -DWIN32_LEAN_AND_MEAN -D__DEBUG__ -D_WIN32 -D_WIN32_WINNT -D__USE_W32_SOCKETS -D__CYGWIN__ -D__SunOS -D__MINGW32__ -D__MSABI_LONG=long -std=gnu++11  
CFLAGS=-c -Wall 
LDFLAGS=-lws2_32 -lboost_system-mt -lboost_signals -lboost_thread -lboost_filesystem 
SOURCES=1_1_1_CreateProfile_test.cpp ../../1_1_1_CreateProfile.hpp ../../md5.h ../../1_1_1_CreateProfile.cpp ../../../../DataEncoderDecoder/DataEncoderDecoder/DataEncoder.h ../../../../DataEncoderDecoder/DataEncoderDecoder/DataEncoder.cpp ../../../../DataEncoderDecoder/DataEncoderDecoder/compression-lib/compression.h ../../../../DataEncoderDecoder/DataEncoderDecoder/compression-lib/compression.cpp ../../../../websocket_server/wsclient/wsclient.cpp ../../../../websocket_server/wsclient/wsclient.hpp ../../../../websocket_server/dataframe.hpp ../../../../websocket_server/dataframe.cpp ../../../../websocket_server/dataframe_parser.hpp ../../../../websocket_server/dataframe_parser.cpp ../../../../websocket_server/header.hpp ../../../../websocket_server/participant.hpp ../../../../websocket_server/reply.hpp ../../../../websocket_server/reply.cpp ../../../../websocket_server/request.hpp ../../../../websocket_server/request_handler.hpp ../../../../websocket_server/request_handler.cpp ../../../../websocket_server/request_parser.hpp ../../../../websocket_server/request_parser.cpp ../../../../websocket_server/room.hpp ../../../../websocket_server/room.cpp ../../../../websocket_server/server.hpp ../../../../websocket_server/server.cpp ../../../../websocket_server/session.hpp ../../../../websocket_server/session.cpp 
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=bin/Debug/1_1_1_test
TEST=./bin/Debug/1_1_1_test.exe 
TESTFLAGS= --report_format=XML --report_level=detailed
CONVERT=xsltproc -o test_results.html ../test_results.xslt 1_1_1_test_results.xml 


all: $(SOURCES) $(EXECUTABLE) test 
	
$(EXECUTABLE): $(OBJECTS) 
	$(CC) $(OBJECTS) -o $@ $(LDFLAGS) 

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@

test:
	$(TEST)$(TESTFLAGS) > error.txt;$(CONVERT) 

clean:
	rm *.o
