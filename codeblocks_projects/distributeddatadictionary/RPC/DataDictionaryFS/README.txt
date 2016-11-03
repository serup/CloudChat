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
    To develop on the client, then make changes to general class RPCclient
    this client should be used in DDDAdmin for handling DDNode's
----

INTERNAL :
A list with connected RPCclients with message ques (ringbuffers)                                  
a RPCclient will connect to server and ask for requests from its message que                                  
this construction is to allow RPCclients to be unflexible in their connection to server                       
in other words a RPCclient should be able to die and others should together with server be able               
to reestablish data lying on the downed RPCclient - this is done from redundant replicas of .BFi files        
on each RPCNode                                                                                               
each Node has up to 3 backup nodes in forward chain - example ring with 3 nodes:                              
node1->node2->node3; node2->node3->node4; node3->node1->node2                                                 
read as :                                                                                                     
  node1 backup on node2 and node3                                                                             
  node2 backup on node3 and node1                                                                             
  node3 backup on node1 and node2                                                                             
	                                                                                                                
----

