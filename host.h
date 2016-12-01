#ifndef __HOST__
#define __HOST__

struct _host
{
	char *ip;
	unsigned int port;
	char *file;
};

typedef struct _host host_t;
#endif
