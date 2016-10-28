######################################################################
#                                                                    #
# Make file for DDDAdmin - Distributed Data Dictionary Administrator #
#                                                                    #
######################################################################
CC=g++ -g -o bin/Debug/DDDAdmin main.cpp utils.hpp utils.cpp datadictionarycontrol.hpp datadictionarycontrol.cpp md5.h ../DataEncoderDecoder/DataEncoderDecoder/DataEncoder.h ../DataEncoderDecoder/DataEncoderDecoder/DataEncoder.cpp ../DataEncoderDecoder/DataEncoderDecoder/compression-lib/compression.h ../DataEncoderDecoder/DataEncoderDecoder/compression-lib/compression.cpp -L"/usr/local/lib/" -lboost_regex -D__DEBUG__ -D__MSABI_LONG=long -DBOOST_LOG_DYN_LINK -lboost_system -lboost_log -lboost_signals -lboost_thread -lboost_filesystem -lpthread -lrt -std=gnu++11 -lcurses -I /tclap-1.2.1/include

info:
	@ echo "------------------------------"
	@ echo " DDDAdmin build "
	@ echo "------------------------------"
	@ echo "to build all : make -f makefile_ubuntu.mak total"
	@ echo "to fetch and build TCLAP : make -f makefile_ubuntu.mak dep"
	@ echo "to install : make -f makefile_ubuntu.mak install"
	@ echo " "

all: compile 
total: compile 
_total: nooutputcompile
dep: depend


depend:
	@ echo "- remove old version of TCLAP"
	-@ rm -rf tclap*
	@ echo "- Download TCLAP for handling commandline commands"
	@ wget https://sourceforge.net/projects/tclap/files/tclap-1.2.1.tar.gz/download -O tclap-1.2.1.tar.gz
	@ echo "- extract "
	@ tar -xvf tclap-1.2.1.tar.gz
	@ echo "- configure, make and install "
	@ ./tclap-1.2.1/configure; make ; sudo make install
	@ echo "- DONE. "


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

install:
	@ echo "- installing DDDAdmin in /usr/local/bin"
	@ sudo cp bin/Debug/DDDAdmin /usr/local/bin/DDDAdmin

.PHONY: info
