#
# http://mrbook.org/tutorials/make/
# http://www.cs.northwestern.edu/academics/courses/211/html/make.html
# http://www.cs.umd.edu/class/fall2002/cmsc214/Tutorial/makefile.html
#
CC=g++ -g -o bin/Debug/scanvaserver main.cpp dataframe.cpp dataframe_parser.cpp reply.cpp request_handler.cpp request_parser.cpp room.cpp server.cpp session.cpp dataframe.hpp dataframe_parser.hpp header.hpp participant.hpp reply.hpp request.hpp request_handler.hpp request_parser.hpp room.hpp server.hpp session.hpp ../DataEncoderDecoder/DataEncoderDecoder/compression-lib/compression.cpp ../DataEncoderDecoder/DataEncoderDecoder/DataEncoder.cpp -L"/usr/local/lib/" -D__MSABI_LONG=long -D__DEBUG -lboost_system -lboost_signals -lboost_thread -lpthread -ldl -std=gnu++11 > error.txt
CFLAGS= --coverage
all: compile 

compile:
	@echo -start building websocket server DEBUG mode -- please wait ...
	@$(CC) $(CFLAGS) 
	@echo -build finished. See error.txt for detail of build

