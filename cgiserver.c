#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include "util.h"
#include "host.h"

void html_head()
{
	printf("Content-Type: text/html\n");
	printf("\n");
	printf("<html>\n");
	printf("<head>\n");
	printf("<meta http-equiv=\"Content-Type\" content=\"text/html; charset=big5\" />\n");
	printf("<title>Network Programming Homework 3</title>\n");
	printf("</head>\n");
	printf("<body bgcolor=#336699>\n");
	printf("<font face=\"Courier New\" size=2 color=#FFFF99>\n");
	printf("<table width=\"800\" border=\"1\">\n");
}

int main()
{
	setenv("QUERY_STRING", "h1=localhost&p1=3451&f1=t1.txt&h2=&p2=&f2=&h3=&p3=&f3=&h4=&p4=&f4=&h5=&p5=&f5=", 1);

	char *query = getenv("QUERY_STRING");
	char **s_array;
	size_t counter;
	
	split(&s_array, query, "&", &counter);
	//printf("%d\n", counter);
	//printf("%s\n", s_array[1]);
	if (counter != 15)
	{
		printf("wrong input format!\n");
		return 0;
	}

	host_t *host[6];

	//init host
	int i = 0;
	for (;i < 6; i++)
		host[i] = NULL;

	i = 1;
	for (;i < 6; i++)
	{
		host_t *temp = malloc(sizeof(host_t));
		memset(temp, 0, sizeof(host_t));

		int index = i - 1;
		if (strlen(s_array[index*3]) > 3 && strlen(s_array[index*3 + 1]) > 3 && strlen(s_array[index*3 + 2]) > 3)
		{
			strcpy(temp->ip, &s_array[index*3][3]);
			temp->port = atoi(&s_array[index*3+1][3]);
			strcpy(temp->file, &s_array[index*3+2][3]);
			host[i] = temp;
		}
	
		else
			free(temp);

	}

	//open non-blocking socket 

	fd_set afds, rfds;
	int nfds = getdtablesize();
	FD_ZERO(&afds);

	for (i = 1; i < 6; i++)
	{
		if (host[i] != NULL)
		{
			int s = socket(AF_INET , SOCK_STREAM , 0);
			struct sockaddr_in server;

			memset(&server, 0, sizeof(server));
			server.sin_addr.s_addr = inet_addr(host[i]->ip);
			server.sin_family = AF_INET;
			server.sin_port = htons(host[i]->port);
			int flag = fcntl(s, F_GETFL, 0);
			fcntl(s, F_SETFL, flag | O_NONBLOCK);
			connect(s, (struct sockaddr *)&server , sizeof(server));

			//set to host
			host[i]->sock_fd = s;
			host[i]->server = server;

			//set afds
			FD_SET(s, &afds);
		}
	}

	//html

	html_head();

	printf("<tr>\n");
	for (i = 1; i < 6; i++)
	{
		if (host[i] != NULL)
		{
			printf("<td>");
			printf("%s", host[i]->ip);
			printf("</td>");
		}
	}
	printf("</tr>\n");

	printf("<tr>\n");
	for (i = 1; i < 6; i++)
	{
		if (host[i] != NULL)
		{
			printf("<td");
			printf(" valign=\"top\" id=\"m%d\"", i-1);
			printf("></td>");
		}
	}
	printf("</tr>\n");
	printf("/table\n");

	printf("</html>\n");

	

	
	/*
	i = 0;
	for (; i < 6; i++)
	{
		if (host[i] != NULL)
		{
			printf("%s\n", host[i]->ip);
			printf("%d\n", host[i]->port);
			printf("%s\n", host[i]->file);
			printf("%d\n", host[i]->sock_fd);
		}
	}
	*/
}
