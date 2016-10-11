const MAXLEN = 65535;

struct DEDBlock {
	long transID;
	opaque data<MAXLEN>;
};

enum requestType {CREATE, UPDATE, DELETE, APPEND, SEARCH, MAPREDUCE};

struct DDRequest {
	/* string data<>; */
	int key;
	requestType reqType;
	/*DDRequest *next;*/
};

 
program DDD_FS_PROG {
	version DDD_FS_VERS {
		DEDBlock DDDfs(void) = 0;
		DEDBlock DDNode(DDRequest) = 1;
	} = 1;
} = 0x30000824;
