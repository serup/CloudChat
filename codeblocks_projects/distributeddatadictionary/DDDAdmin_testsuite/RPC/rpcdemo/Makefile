.SUFFIXES:
.SUFFIXES: .c .o
CLNT = llist
SRVR = llist_svc
CFLAGS = -g -Wall

SRVR_OBJ = llist_svc_proc.o llist_xdr.o llist_svc.o
CLNT_OBJ = llist.o llist_xdr.o llist_clnt.o

.c.o:; gcc -c -o $@ $(CFLAGS) $<

init:
	rpcgen llist.x

all: $(CLNT) $(SRVR)

$(CLNT): $(CLNT_OBJ) llist.h
	gcc -o $(CLNT) $(CLNT_OBJ)

$(SRVR): $(SRVR_OBJ) llist.h
	gcc -o $(SRVR) $(SRVR_OBJ)


clean:
	-@rm llist.h llist_svc.c llist_xdr.c llist_clnt.c
	-@rm *.o $(CLNT) $(SRVR)
#	-@rm -i *~
#	-@rm core
