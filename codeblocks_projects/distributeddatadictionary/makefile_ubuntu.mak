######################################################################
#                                                                    #
# Make file for DDDAdmin - Distributed Data Dictionary Administrator #
#                                                                    #
######################################################################
CC=g++ -g -o bin/Debug/DDDAdmin main.cpp datadictionarycontrol.hpp datadictionarycontrol.cpp md5.h ../DataEncoderDecoder/DataEncoderDecoder/DataEncoder.h ../DataEncoderDecoder/DataEncoderDecoder/DataEncoder.cpp ../DataEncoderDecoder/DataEncoderDecoder/compression-lib/compression.h ../DataEncoderDecoder/DataEncoderDecoder/compression-lib/compression.cpp -L"/usr/local/lib/" -D__DEBUG__ -D__MSABI_LONG=long -DBOOST_LOG_DYN_LINK -lboost_system -lboost_log -lboost_signals -lboost_thread -lboost_filesystem -lpthread -lrt -std=gnu++11

info:
	@ echo "------------------------------"
	@ echo " DDDAdmin build "
	@ echo "------------------------------"
	@ echo "to build all : make -f makefile_ubuntu.mak total"
	@ echo " "

all: compile 
total: compile 
_total: nooutputcompile

nooutputcompile:
	@ $(CC)
compile:
	@ echo "------------------------"
	@ echo "create output dir "
	@ mkdir -p bin/Debug
	@ echo "Build started -- please wait..."	
	@ $(CC)
	@ echo "Build ended --"
	@ echo "------------------------"

.PHONY: info
