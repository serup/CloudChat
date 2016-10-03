init:
	@echo "----------------------------------------------"
	@echo " Build RPC server/client for DDD file system "
	@echo "----------------------------------------------"
	@echo " - make all : build all"
	@echo " - clean : make clean "
	@echo "----------------------------------------------"

all: client server

DDDfs.h: DDDfs.x
	rpcgen DDDfs.x

DDDfs_svc.c DDDfs_clnt.c DDDfs.c: DDDfs.h

client: DDDfs.o DDDfs_clnt.o
	cc -o DDDfsclient DDDfs.o DDDfs_clnt.o  -lnsl

server: DDDfs_server.o DDDfs_svc.o
	cc -o DDDfsserver DDDfs_server.o DDDfs_svc.o -lnsl

.PHONY: clean

clean:
	-rm *.o
	-rm *client
	-rm *server
	-rm *fs.h
	-rm *fs_clnt.c
	-rm *fs_svc.c
