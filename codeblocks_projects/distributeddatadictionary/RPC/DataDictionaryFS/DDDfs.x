const MAXLEN = 65535;

struct DEDBlock {
	long transID;
	opaque data<MAXLEN>;
};


enum requestType {PINGPONG, CONNECT, CREATE, UPDATE, DELETE, APPEND, SEARCH, MAPREDUCE};

struct DDRequest {
	requestType reqType;
	DEDBlock ded;
};

 
program DDD_FS_PROG {
	version DDD_FS_VERS {
		DEDBlock DDDfs(DDRequest) = 0;
		DEDBlock DDNode(DDRequest) = 1;
	} = 1;
} = 0x30000824;
