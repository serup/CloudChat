######################################################################
#                                                                    #
# Make file for RPC - generates rpcgen files                         #
#                                                                    #
######################################################################
info:
	@ echo "------------------------------"
	@ echo " RPC - generate rpcgen files  "
	@ echo "------------------------------"
	@ echo ""
	@ echo "to build: make all"
	@ echo "to clear: make clear"
	@ echo "------------------------------"

all: compile 
compile:
	@ echo "------------------------"
	@ echo "Generate files -- please wait..."	
	@ rpcgen *.x
	@ echo "Build ended --"
	@ echo "------------------------"
clear:
	@ echo "Clean up"
	@ rm *.h *.c
.PHONY: info
