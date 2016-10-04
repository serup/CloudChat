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

- and you can then compile and link using
make -f Makefile.DDDfs

