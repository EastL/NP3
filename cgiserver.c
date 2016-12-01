#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "util.h"
#include "host.h"

int main()
{
	setenv("QUERY_STRING", "h1=localhost&p1=3451&f1=t1.txt", 1);
	printf("Content-Type: text/html\n\n");

	char *query = getenv("QUERY_STRING");
	char **s_array;
	size_t counter;
	
	split(&s_array, query, "&", &counter);
	//printf("%d\n", counter);
	//printf("%s\n", s_array[1]);
	if (counter % 3 != 0)
	{
		printf("wrong input format!\n");
		return 0;
	}

	host_t *host[6];

	//init host
	int i = 0;
	for (;i < 6; i++)
		host[i] = NULL;

	i = 0;
	//for (;i < counter;, i++)
	//{
		
	//}
}
