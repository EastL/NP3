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
#include <errno.h>
#include "util.h"
#include "host.h"

void html_head()
{
	printf("Content-Type: text/html\n\n");
	fflush(stdout);
	printf("<html>\n");
	fflush(stdout);
	printf("<head>\n");
	fflush(stdout);
	printf("<meta http-equiv=\"Content-Type\" content=\"text/html; charset=big5\" />\n");
	fflush(stdout);
	printf("<title>Network Programming Homework 3</title>\n");
	fflush(stdout);
	printf("</head>\n");
	fflush(stdout);
	printf("<body bgcolor=#336699>\n");
	fflush(stdout);
	printf("<font face=\"Courier New\" size=2 color=#FFFF99>\n");
	fflush(stdout);
	printf("<table width=\"800\" border=\"1\">\n");
	fflush(stdout);
}

int main()
{
	//setenv("QUERY_STRING", "h1=127.0.0.1&p1=13420&f1=t1.txt&h2=&p2=&f2=&h3=&p3=&f3=&h4=&p4=&f4=&h5=&p5=&f5=", 1);

	char *query = getenv("QUERY_STRING");
	char **s_array;
	size_t counter;
	int nhost = 0;
	
	printf("q:%s\n", query);
	chdir("/u/other/2017_1/104522052/public_html");
	split(&s_array, query, "&", &counter);
	//printf("%d\n", counter);
	printf("arr1:%s\n", s_array[0]);
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

	for (i = 1; i < 6; i++)
	{
		host_t *temp = malloc(sizeof(host_t));
		memset(temp, 0, sizeof(host_t));

		int index = i - 1;
		if (strlen(s_array[index*3]) > 3 && strlen(s_array[index*3 + 1]) > 3 && strlen(s_array[index*3 + 2]) > 3)
		{
			//strcpy(temp->ip, &s_array[index*3][3]);
			strcpy(temp->ip, "140.113.168.193");
			//temp->port = atoi(&s_array[index*3+1][3]);
			temp->port = 13420;
			strcpy(temp->file, &s_array[index*3+2][3]);
			temp->file_fd = fopen(temp->file, "r");
			temp->connected = 0;
			host[i] = temp;
			nhost++;
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

			if (connect(s, (struct sockaddr *)&server , sizeof(server)) < 0)
			{
				if (errno != EINPROGRESS)
				{
					printf("Errno : %d\n", errno);
					printf("<h2>connect error.</h2>\n");
					fflush(stdout);
					return -1;
				}
			}
	
			else
				host[i]->connected = 1;

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
			fflush(stdout);
			printf("%s", host[i]->ip);
			fflush(stdout);
			printf("</td>");
			fflush(stdout);
		}
	}
	printf("</tr>\n");
	fflush(stdout);

	printf("<tr>\n");
	fflush(stdout);
	for (i = 1; i < 6; i++)
	{
		if (host[i] != NULL)
		{
			printf("<td");
			fflush(stdout);
			printf(" valign=\"top\" id=\"m%d\"", i-1);
			fflush(stdout);
			printf("></td>");
			fflush(stdout);
		}
	}
	printf("</tr>\n");
	fflush(stdout);
	printf("</table>\n");
	fflush(stdout);

	int complete = 0;
	while (1)
	{
		memcpy(&rfds, &afds, sizeof(afds));
		select(nfds, &rfds, NULL, NULL, NULL);

		for (i = 1; i < 6; i++)
		{
			if (host[i] != NULL && host[i]->connected == 0)
			{
				if (connect(host[i]->sock_fd, (struct sockaddr *)&(host[i]->server), sizeof(host[i]->server)) < 0)
				{
					if(errno == EISCONN)
						host[i]->connected = 1;	
				}
			}

			if (host[i] != NULL && FD_ISSET(host[i]->sock_fd, &rfds))
			{
				//recv msg, end with /r/n
				char msg[10010];
				memset(msg, 0, 10010);
				int counter = 0;
				char buf[2];
				memset(buf, 0, 2);
				while (read(host[i]->sock_fd, buf, 1) > 0)
				{
					if (buf[0] == '\r')
						continue;
					msg[counter++] = buf[0];
					if (buf[0] == '\n')
						break;
				}
				

				if (counter == 0)
					continue;

				//output msg
				//must print cmd
				if (strncmp(msg, "% ", 2) == 0)
				{
					//read cmd file
					char next_cmd[10010];
					memset(next_cmd, 0, 10010);
					fgets(next_cmd, 10010, host[i]->file_fd);
					usleep(100000);
					write(host[i]->sock_fd, next_cmd, strlen(next_cmd));
					if (strncmp(next_cmd, "exit", 4) == 0)
					{
						complete++;
						FD_CLR(host[i]->sock_fd, &afds);
						close(host[i]->sock_fd);
						free(host[i]);
						host[i] = NULL;
					}
					replace_html(next_cmd);
					printf("<script>document.all['m%d'].innerHTML += \"%% <b> %s</b>\";</script>\n", i-1, next_cmd);
					fflush(stdout);
				}
				
				else
				{
					replace_slash(msg);
					replace_html(msg);
					printf("<script>document.all['m%d'].innerHTML += \"%s\";</script>\n", i-1, msg);
					fflush(stdout);
				}
			}
		}

		if (complete == nhost)
			break;
	}
	

	
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
	return 0;
}
