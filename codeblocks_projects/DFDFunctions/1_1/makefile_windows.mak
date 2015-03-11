#
# http://mrbook.org/tutorials/make/
# http://www.cs.northwestern.edu/academics/courses/211/html/make.html
# IMPORTANT THIS IS A WINDOWS STATIC LINKED LIBS VERSION, HOWEVER IT STILL NEEDS THE cygwin1.dll FILE TO BE PRESENT CLOSE TO ITS EXECUTABLE - OTHER LIBS ARE STATICALLY LINKED -- you could ofcours just add the c:\cygwin\bin  to your windows path variable, then it should be ok if you have cygwin installed, if not then you need to distribute the cygwin1.dll file together with your file
#
# http://www.querycat.com/question/a0ad45275f0bb6059cc0439b33622088 
# Can I build a Cygwin program that does not require cygwin1.dll at runtime?
# Cygwin FAQ
# No. If your program uses the Cygwin API, then your executable cannot run without cygwin1.dll. In particular, it is not possible to statically link with a Cygwin library to obtain an independent, self-contained executable. If this is an issue because you intend to distribute your Cygwin application, then you had better read and understand http://cygwin.com/licensing.html, which explains the licensing options.
# http://cygwin.com/faq/
#
#
# http://www.cs.umd.edu/class/fall2002/cmsc214/Tutorial/makefile.html
#
# IMPORTANT! HAVE LINKS AFTER THE OBJECTS FILES, LINK ORDER MATTERS
# TODO: try to make this makefile fetch obj files from obj folder, thus making
# it compatible with the C::B project build - sofar I did not succeed in this
CC=g++ -g -L"/cygdrive/c/cygwin/usr/local/lib/" -DWIN32_LEAN_AND_MEAN -D__DEBUG__ -D_WIN32 -D_WIN32_WINNT -D__USE_W32_SOCKETS -D__CYGWIN__ -D__SunOS -D__MINGW32__ -D__MSABI_LONG=long -std=gnu++11  
CFLAGS=-c -Wall -mno-cygwin 
LDFLAGS=-static -lws2_32 -lboost_system -lboost_signals -lboost_thread -lboost_filesystem 
SOURCES=main.cpp 1_1_Profile.hpp md5.h 1_1_Profile.cpp ../../DataBaseControl/databasecontrol.hpp ../../DataBaseControl/databasecontrol.cpp ../../DataEncoderDecoder/DataEncoderDecoder/DataEncoder.h ../../DataEncoderDecoder/DataEncoderDecoder/DataEncoder.cpp ../../DataEncoderDecoder/DataEncoderDecoder/compression-lib/compression.h ../../DataEncoderDecoder/DataEncoderDecoder/compression-lib/compression.cpp ../../websocket_server/wsclient/wsclient.cpp ../../websocket_server/wsclient/wsclient.hpp ../../websocket_server/dataframe.hpp ../../websocket_server/dataframe.cpp ../../websocket_server/dataframe_parser.hpp ../../websocket_server/dataframe_parser.cpp ../../websocket_server/header.hpp ../../websocket_server/participant.hpp ../../websocket_server/reply.hpp ../../websocket_server/reply.cpp ../../websocket_server/request.hpp ../../websocket_server/request_handler.hpp ../../websocket_server/request_handler.cpp ../../websocket_server/request_parser.hpp ../../websocket_server/request_parser.cpp ../../websocket_server/room.hpp ../../websocket_server/room.cpp ../../websocket_server/server.hpp ../../websocket_server/server.cpp ../../websocket_server/session.hpp ../../websocket_server/session.cpp 
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=bin/Debug/1_1.exe


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
