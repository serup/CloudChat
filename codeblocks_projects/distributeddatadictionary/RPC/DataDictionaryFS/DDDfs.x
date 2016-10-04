const MAXLEN = 65535;

struct DEDBlock {
	long sizeofBlock;
	opaque data<MAXLEN>;
};

 
program HELLO_WOLRD_PROG {
	version HELLO_WORLD_VERS {
		DEDBlock DDDfs(void) = 0;
	} = 1;
} = 0x30000824;
