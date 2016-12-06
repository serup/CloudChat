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

when a RPCclient connects to DDDAdmin, then it also connects to ZooKeeper and adds itself in EPHEMERAL mode, with information on which files it has, this
means when it disconnects other nodes will no longer see info in ZooKeeper about that client, and a new leader of ZooKeeper nodes will be elected and an
event will cause a warning, and if node (RPCclient) does not come back, then a recovery should be atomatically started, where replicas will be used to re-
establish lost .BFi files on a NEW node (when a new RPCclient is added with request to takeover old RPCclient)


                                                 DDDAdmin           [zookeeper Server]
                                                    | zkClient
     +------------------+---------------------------+-----------------------------------------------------------+
	 |                  |                
   [RPCclient]      [RPCclient]
      zkClient         zkClient


each file on a RPCclient node is accessed via zookeeper, when creating, updating, moving or deleting - the zookeeper
keeps track of where the file is and this info is stored in the zookeeper filesystem - minimum metadata

When a RPCclient wants to create a file it does so using connection with DDDAdmin and via zookeeper connection to zookeeper Server
The RPCclient registers to DDDAmin and DDDAdmin stores info in zookeeper about the registration. The DDDAdmin monitors all
RPCclients via zookeeper, and so it is the DDDAdmin's responsibility to make sure that the integrity of the system is without errors

When Distributed Data Dictionary is running, then it is possible to use a DDDAdmin to connect to zookeeper and ask info about the integrity of the 
Distributed Data Dictionary (all the RPCclient nodes and their data files .BFi) - also it is possible to manually move .BFi files around, however
it is DDDAdmin's prime function to move these files around - each RPCclient will give info to DDDAdmin via zookeeper or via Request/Response tcp calls

Scenario: lets say that a large file is stored in multiple .BFi files, then the DDDAdmin is notified about this new creation and orders some of the files 
to be distributed to other nodes (RPCclients), thereby making sure that the replica area and footprint of file on nodes is handled, meaning that it
orders copy and move of these .BFi files ( it is important that the redundant replicas of .BFi is put in correct places, and that zookeeper is updated )

The DDDAdmin can ask via zookeeper a RPCclient to connect to it, it can then use this connection to handle request/reponses comming to and from the client.
the zookeeper tells the RPCclient where to connect to DDDAdmin. The reason for this is to avoid having ALL clients connected to DDDAdmin at the same time,
it is only necessary to have a tcp "pipe" from/to DDDAdmin and RPCclient when a transfer of administrative request/responses is handled

Scenario: lets say that multiple DDDAdmin's are connected to zookeeper and asking RPCclients to do various things - inorder for this to work, then
it must be zookeeper that decides when access is given and when a certain action is to be performed by the RPCclient


NB! consider creating a virtual filehandle vector, meaning a vector containing filehandle from each .BFi file 
- this filehandle vector should be able to be used
via a function in DDDAdmin library, as a normal filehandle would be used; example :

[RPC]    [RPC]    [RPC]
h--------h----^---h----------------------------------

h = handle start of file in session on RPCclient 

- a file could be inside a .BFi, however the RPC makes a session and creates a virtual file and h is 
handle to that file, so whenever user of that open session file is moving it, then RPCclient will move
it and if it goes over current .BFi in current RPCclient, then it will tell zookeeper where next handle is
and on which RPCclient and this is done in the virtual filehandle vector, so user can read from file 
as if it was a normal contiquous file

^ = position of the virtual file handle for the large file




NB! consider using redox/redis database cache together with zookeeper to handle transfer of bulk data between RPCnodes
    info on redox : https://github.com/hmartiro/redox
	                Redox is a C++ interface to the Redis key-value store that makes it easy to write applications that are both elegant and high-performance.
	info on redis : https://redis.io/topics/introduction  
	                Redis is an open source (BSD licensed), in-memory data structure store, used as a database, cache and message broker. 
					It supports data structures such as strings, hashes, lists, sets, sorted sets with range queries, bitmaps, hyperloglogs and geospatial indexes with radius queries.
----

