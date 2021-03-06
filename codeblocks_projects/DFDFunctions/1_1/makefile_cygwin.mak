#
# http://mrbook.org/tutorials/make/
# http://www.cs.northwestern.edu/academics/courses/211/html/make.html
# http://www.cs.umd.edu/class/fall2002/cmsc214/Tutorial/makefile.html
#
# IMPORTANT! HAVE LINKS AFTER THE OBJECTS FILES, LINK ORDER MATTERS
# TODO: try to make this makefile fetch obj files from obj folder, thus making
# it compatible with the C::B project build - sofar I did not succeed in this
CC=g++ -g -L"/cygdrive/c/cygwin/usr/local/lib/" -DWIN32_LEAN_AND_MEAN -D__DEBUG__ -D_WIN32 -D_WIN32_WINNT -D__USE_W32_SOCKETS -D__CYGWIN__ -D__SunOS -D__MINGW32__ -D__MSABI_LONG=long -std=gnu++11 -U__STRICT_ANSI__
CFLAGS=-c -Wall 
LDFLAGS=-lws2_32 -lboost_system-mt -lboost_signals -lboost_thread -lboost_filesystem 
SOURCES=main.cpp 1_1_Profile.hpp md5.h base64.h base64.cpp 1_1_Profile.cpp ../../DataBaseControl/databasecontrol.hpp ../../DataBaseControl/databasecontrol.cpp ../../DataEncoderDecoder/DataEncoderDecoder/DataEncoder.h ../../DataEncoderDecoder/DataEncoderDecoder/DataEncoder.cpp ../../DataEncoderDecoder/DataEncoderDecoder/compression-lib/compression.h ../../DataEncoderDecoder/DataEncoderDecoder/compression-lib/compression.cpp ../../websocket_server/wsclient/wsclient.cpp ../../websocket_server/wsclient/wsclient.hpp ../../websocket_server/dataframe.hpp ../../websocket_server/dataframe.cpp ../../websocket_server/dataframe_parser.hpp ../../websocket_server/dataframe_parser.cpp ../../websocket_server/header.hpp ../../websocket_server/participant.hpp ../../websocket_server/reply.hpp ../../websocket_server/reply.cpp ../../websocket_server/request.hpp ../../websocket_server/request_handler.hpp ../../websocket_server/request_handler.cpp ../../websocket_server/request_parser.hpp ../../websocket_server/request_parser.cpp ../../websocket_server/room.hpp ../../websocket_server/room.cpp ../../websocket_server/server.hpp ../../websocket_server/server.cpp ../../websocket_server/session.hpp ../../websocket_server/session.cpp 
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=bin/Debug/DFD_1_1.exe


all:	msgbegin compile msgend

msgbegin:
	@ echo "------------------------"
	@ echo "Build started -- please wait..."	

compile: $(SOURCES) $(EXECUTABLE)  

msgend:
	@ cat error.txt
	@ echo "Build ended --"
	@ echo "------------------------"
	
$(EXECUTABLE): $(OBJECTS) 
	@ ($(CC) $(OBJECTS) -o $@ $(LDFLAGS) > error.txt) > tmp
	@ echo "Linking " $@
.cpp.o:
	@ $(CC) $(CFLAGS) $< -o $@
	@ echo "Compiling " $@

clean:
	rm *.o
