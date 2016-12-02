#ifndef __HOST__
#define __HOST__

struct _host
{
	char ip[20];
	unsigned int port;
	char file[30];
};

typedef struct _host host_t;
#endif
