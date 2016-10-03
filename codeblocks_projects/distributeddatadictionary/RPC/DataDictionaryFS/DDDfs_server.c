#include <rpc/rpc.h>
#include "DDDfs.h"

/* 
 *    Hello world RPC server -- it just returns the string.
 *    */

char **dddfs_1_svc(void *a, struct svc_req *req) {
	static char msg[256];
	static char *p;

	printf("getting ready to return value\n");
	strcpy(msg, "Hello world");
	p = msg;
	printf("Returning...\n");

	return(&p);
}
