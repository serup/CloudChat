#
# http://mrbook.org/tutorials/make/
# http://www.cs.northwestern.edu/academics/courses/211/html/make.html
# http://www.cs.umd.edu/class/fall2002/cmsc214/Tutorial/makefile.html
#
CC=g++ -g -o bin/Debug/scanvaserver main.cpp dataframe.cpp dataframe_parser.cpp reply.cpp request_handler.cpp request_parser.cpp room.cpp server.cpp session.cpp dataframe.hpp dataframe_parser.hpp header.hpp participant.hpp reply.hpp request.hpp request_handler.hpp request_parser.hpp room.hpp server.hpp session.hpp ../DataEncoderDecoder/DataEncoderDecoder/compression-lib/compression.cpp ../DataEncoderDecoder/DataEncoderDecoder/DataEncoder.cpp  -L"/cygdrive/c/cygwin/usr/local/lib/" -DWIN32_LEAN_AND_MEAN -D_WIN32_WINNT -D__USE_W32_SOCKETS -D__CYGWIN__ -D__SunOS -D__MINGW32__ -D__MSABI_LONG=long -D__DEBUG -lws2_32 -lboost_system.dll -lboost_signals.dll -lboost_thread.dll -std=gnu++11 > error.txt

all: compile 

compile:
	@echo -start building websocket server in DEBUG mode -- please wait
	@$(CC)
	@echo -build finished. See error.txt for detail of build

