#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "util.h"
#include "host.h"

void html_header()
{
	printf("<head>\n");
	printf("<meta http-equiv=\"Content-Type\" content=\"text/html; charset=big5\" />\n");
	printf("<title>Network Programming Homework 3</title>\n");
	printf("</head>\n");
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

	//connect to host

	

	/*
	i = 0;
	for (; i < 6; i++)
	{
		if (host[i] != NULL)
		{
			printf("%s\n", host[i]->ip);
			printf("%d\n", host[i]->port);
			printf("%s\n", host[i]->file);
		}
	}
	*/
}
