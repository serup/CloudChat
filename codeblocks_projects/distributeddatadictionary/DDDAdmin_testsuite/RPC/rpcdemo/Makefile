.SUFFIXES:
.SUFFIXES: .c .o
CLNT = llist
SRVR = llist_svc
CFLAGS = -g -Wall
RPCGEN_FLAG = -C 

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

# make sure we regenerate them if our interface (add .x) changes  
llist_clnt.c llist_svc.c llist_xdr.c llist.h: llist.x  
	rpcgen $(RPCGEN_FLAG) llist.x  

clean:
	-@rm llist.h llist_svc.c llist_xdr.c llist_clnt.c
	-@rm *.o $(CLNT) $(SRVR)
