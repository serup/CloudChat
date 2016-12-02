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
ZooKeeper is used to keep track of clients data - it contains metadata on where and what

this construction is to allow RPCclients to be unflexible in their connection to server                       
in other words a RPCclient should be able to die and others should together with server be able               
to reestablish data lying on the downed RPCclient - this is done from redundant replicas of .BFi files        
on each RPCNode                                                                                               
each Node has up to 3 backup nodes in forward chain - example ring with 3 nodes:                              
node1->node2->node3; node2->node3->node1; node3->node1->node2                                                 
read as :                                                                                                     
  node1 backup on node2 and node3                                                                             
  node2 backup on node3 and node1                                                                             
  node3 backup on node1 and node2                                                                             

NB! consider using redox/redis database cache together with zookeeper to handle transfer of bulk data between RPCnodes
    info on redox : https://github.com/hmartiro/redox
	                Redox is a C++ interface to the Redis key-value store that makes it easy to write applications that are both elegant and high-performance.
	info on redis : https://redis.io/topics/introduction  
	                Redis is an open source (BSD licensed), in-memory data structure store, used as a database, cache and message broker. 
					It supports data structures such as strings, hashes, lists, sets, sorted sets with range queries, bitmaps, hyperloglogs and geospatial indexes with radius queries.
----

