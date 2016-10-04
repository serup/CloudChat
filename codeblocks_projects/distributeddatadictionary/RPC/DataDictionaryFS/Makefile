init:
	@echo "----------------------------------------------"
	@echo " Build RPC server/client for DDD file system "
	@echo "----------------------------------------------"
	@echo " - make generate : generate client and server sample code - use only first time"
	@echo " - make all : build all"
	@echo " - clean : make clean "
	@echo "----------------------------------------------"

setup:	generate prereq

generate:
	rpcgen -Na DDDfs.x

prereq:
	@echo "- then rename all .c to .cpp"
	rename "s/\.c/\.cpp/" *.c 
	@echo "- then rename all .c to .cpp in Makefile.DDDfs"
	sed -i 's/\.c/\.cpp/g' Makefile.DDDfs
	@echo "- and you can then compile and link using"

all:
	make -f Makefile.DDDfs 

.PHONY: init 

clean:
	make -f Makefile.DDDfs clean
	rm Makefile.DDDfs
