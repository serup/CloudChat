DDD - Distributed Data Dictionary

Data Dictionary is a place for DOPS database files and realm entry description files
each entity in the database is an xml file which contains DDEntry elements described in the Entities files for each realm

Distributed Data Dictionary admin is a central point for APPS to communicate with the database and it is connected to DOPs
webserver (proactor design server) - using DED as datapacket protocol.
the DDDAdmin has several DDNode's each taken care of how the database files are distributed on different machines

it is the role of DDDAdmin of maintaining the data integrity of saved database files and description realm files, hence
handling a list of redundancy of each DDNode block - files will be transformed into blocks of certain size and then every
block will be distributed to a minimum of 3 nodes -- DDDAdmin will maintain a list of where the blocks are, thus enabling
an awareness of what to do if a DDNode fails to connect, or what to do if a new DDNode gets connected instead of an old.
the list however should be possible to recreate by asking connected DDNodes which blocks they have.

DDNode - Data Dictionary Node

This node, can be instantiated many times on different computer systems, and is connected to DDDAdmin, via DOPs websocket proactor server,
and handles blocks of database files, each database file is distributed to several DDNode's in blocks of x-size
it is the role of DDNode to handle map and reduce jobs comming from DDDAdmin.
in parallel the DDNodes search for specific attributes inside the blocks, and if found is returning a key,value to DDDAdmin
The blocks contains xml-nodes where each node is an attribute defined in the Data Dictionary DDEntry for a realm and each
database file is a record belonging to a specific realm

--

Why the need for this?
 map/reduce java jobs using files stored in hadoop file system is working - however extremely slow - the principle is sound and thus 
 building a project in C++ that does the same without java VM overhead and without hadoop filesystem overhead, then perhaps
 performance and speed can bee in the acceptable range - also handling files in a simplified way will make map/reduce jobs
 easier to handle and build, since reading from a database file will be a minimum of reading an entity node of the realms record file.



