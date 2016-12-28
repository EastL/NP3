#include <stdlib.h>
#include <stdio.h>
#include <string.h>

void split(char ***arr, char *str, const char *del, size_t *count)
{
	size_t _count = 0;
	char buf[10010];
	char **_arr = malloc(sizeof(char*) * 5000);
	memset(_arr, 0, sizeof(char*) * 5000);

	strncpy(buf, str, 10010);

	char *save;
	char *s = strtok_r(buf, del, &save);

	while (s != NULL)
	{
		//printf("util:%s\n", s);
		size_t __count = strlen(s);
		*(_arr + _count) = malloc(sizeof(char) * (__count + 1));
		memset(*(_arr + _count), 0, (__count + 1));
		strcpy(*(_arr + _count), s);
		s = strtok_r(NULL, del, &save);
		_count++;
	}

	*count = _count;
	*arr = _arr;
}

void replace_html(char *str)
{
	int count = 0;
	int h_count = 0;
	char html[10010];
	memset(html, 0, 10010);
	
	while(str[count] != '\0')
	{
		if (str[count] == '\n')
		{
			html[h_count++] = '<';
			html[h_count++] = 'b';
			html[h_count++] = 'r';
			html[h_count++] = '>';
		}

		else if (str[count] == '\r');

		else if (str[count] == '>')
		{
			html[h_count++] = '&';
			html[h_count++] = 'g';
			html[h_count++] = 't';
			html[h_count++] = ';';
		}

		else if (str[count] == '<')
		{
			html[h_count++] = '&';
			html[h_count++] = 'l';
			html[h_count++] = 't';
			html[h_count++] = ';';
		}

		else
			html[h_count++] = str[count];

		count++;
	}

	strcpy(str, html);
}

void replace_slash(char *str)
{
	int count = 0;
	int s_count = 0;
	char slash[1024];
	memset(slash, 0, 1024);

	while(str[count] != '\0')
	{
		if (str[count] == '"')
		{
			slash[s_count++] = '\\';
			slash[s_count++] = '"';
		}

		else
			slash[s_count++] = str[count];

		count++;
	}

	strcpy(str, slash);
}
