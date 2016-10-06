const MAXLEN = 65535;

struct DEDBlock {
	long transID;
	opaque data<MAXLEN>;
};

 
program DDD_FS_PROG {
	version DDD_FS_VERS {
		DEDBlock DDDfs(void) = 0;
	} = 1;
} = 0x30000824;
