In order to generate client and server examples from the .x file then do following

rpcgen -Na DDDfs.x

- this will generate many files: 

DDDfs_client.c             
DDDfs_clnt.c                                           
DDDfs.h                    
DDDfs_server.c                                             
DDDfs_svc.c                                                
DDDfs_xdr.c                
Makefile.DDDfs 

- then rename all .c to .cpp
rename "s/\.c/\.cpp/" *.c 

- then rename all .c to .cpp in Makefile.DDDfs
sed -i 's/\.c/\.cpp/g' Makefile.DDDfs

- then add dependencies  - see Makefile for detailed info

- and you can then compile and link using
make -f Makefile.DDDfs


----
all above is done by Makefile when calling
make setup
then you can call
make all
-- if you call 
make destroy
then setup will go back to default from git pull, you can then call make setup again
----

NB! To develop on the server, then make changes ONLY to DDNode.cpp
    server here means server on node
    To develop on the client, then make changes to general class RPCclient
    this client should be used in DDDAdmin for handling DDNode's
----


