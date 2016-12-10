#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>
#include "util.h"

void wait4_child(int signo)
{
	int status;  
	while(waitpid(-1, &status, WNOHANG) > 0);  
}

int read_line(int fd, char* buffer)
{
	int count = 0;
	char buf[1];
	while (read(fd, buf, 1) > 0)
	{
		if (buf[0] == '\r')
			continue;

		buffer[count++] = buf[0];

		if (buf[0] == '\n')
			break;
	}

	return count;
}

void notfound(int fd)
{
	close(0);
	close(1);
	close(2);
	dup(fd);
	dup(fd);
	dup(fd);

	printf("HTTP/1.1 404 Not Found\r\n");
	fflush(stdout);
	printf("Content-type: text/html\r\n\r\n");
	fflush(stdout);
	printf("<html>\n<body>");
	fflush(stdout);
	printf("<img src=\"http://cdn.sstatic.net/Sites/stackoverflow/img/polyglot-404.png\"></img>\n");
	fflush(stdout);
	printf("</body>\n</html>\n");
	fflush(stdout);
}

void exe_cgi(char *url, int fd)
{
	char **arr;
	size_t n;	
	split(&arr, url, "/", &n);
	if (n < 2)
	{
		notfound(fd);
		return;
	}
}


int main()
{
	//kill zombie
	signal(SIGCHLD, wait4_child);

	int sockfd;
	struct sockaddr_in mysocket;

	sockfd = socket(AF_INET, SOCK_STREAM, 0);

	bzero(&mysocket, sizeof(mysocket));
	mysocket.sin_family = AF_INET;
	mysocket.sin_addr.s_addr = INADDR_ANY;
	mysocket.sin_port = htons(13421);

	int sock_opt = 1 ;
	setsockopt(sockfd,SOL_SOCKET,SO_REUSEADDR,(void *)&sock_opt,sizeof(sock_opt));
	bind(sockfd, (struct sockaddr*) &mysocket, sizeof(mysocket));
	listen(sockfd, 20);

	int clientfd;
	struct sockaddr_in client_socket;
	int cl_len = sizeof(client_socket);

	while (1)
	{
		//accept client
		clientfd = accept(sockfd, (struct sockaddr*)&client_socket, &cl_len);

		//fork to deal with http
		int pid = fork();
		if (pid == 0)
		{
			close(sockfd);
			//parse url
			char buf[10010];
			memset(buf, 0, 10010);
			if (read_line(clientfd, buf) > 0)
			{
				//GET url http
				char **arr;
				size_t n;
				split(&arr, buf, " ", &n);
				char url[10010];
				memset(url, 0, 10010);

				if (n == 3)
					strcpy(url, arr[1]);
				
				else
					return -1;

				//set non-blocking
				int flag = fcntl(clientfd, F_GETFL, 0);
				fcntl(clientfd, F_SETFL, flag | O_NONBLOCK);

				while (read_line(clientfd, buf) > 0)
				{
					usleep(100000);
					printf("%s\n", buf);
				}

				//set blocking
				flag = fcntl(clientfd, F_GETFL, 0);
				flag = flag & (~O_NONBLOCK);
				fcntl(clientfd, F_SETFL, flag);

				exe_cgi(url, clientfd);
				close(clientfd);
				return 0;
			}
		}

		else
			close(clientfd);
	}
	
	return 0;
}
