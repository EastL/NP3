#ifndef __HOST__
#define __HOST__

struct _host
{
	char ip[30];
	unsigned int port;
	char file[30];
	FILE *file_fd;
	int sock_fd;
	int connected;
	struct sockaddr_in server;
};

typedef struct _host host_t;
#endif
