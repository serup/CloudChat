init:
	@echo "----------------------------------------------"
	@echo " Build RPC server/client for DDD file system "
	@echo "----------------------------------------------"
	@echo " - make generate : generate client and server sample code - use only first time"
	@echo " - make all : build all"
	@echo " - clean : make clean "
	@echo "----------------------------------------------"

generate:
	rpcgen -Na DDDfs.x

#all: client server
all:	
	make -f Makefile.DDDfs 

DDDfs.h: DDDfs.x
	rpcgen -Na DDDfs.x
#	rpcgen -C DDDfs.x

DDDfs_svc.c DDDfs_clnt.c DDDfs.cpp: DDDfs.h

client: DDDfs.o DDDfs_clnt.o
	g++ -g -o DDDfsclient DDDfs.o DDDfs_clnt.o  

server: DDDfs_server.o DDDfs_svc.o
	g++ -g -o DDDfsserver DDDfs_server.o DDDfs_svc.o 

.PHONY: clean

clean:
	-rm *.o
	-rm *client
	-rm *server
	-rm *fs.h
	-rm *fs_clnt.c
	-rm *fs_svc.c
