#include <windows.h>
#include <list>
#include <cstring>
#include <string.h>
#include <regex>
#include <errno.h>

using namespace std;

#include "resource.h"

#define SERVER_PORT 3421

#define WM_SOCKET_NOTIFY (WM_USER + 1)
#define CGI_SOCKET_NOTIFY (WM_USER + 2)

BOOL CALLBACK MainDlgProc(HWND, UINT, WPARAM, LPARAM);
int EditPrintf (HWND, TCHAR *, ...);
//=================================================================
//	Global Variables
//=================================================================
list<SOCKET> Socks;

struct _host
{
	char ip[30];
	unsigned int port;
	char file[30];
	FILE *file_fd;
	SOCKET sock_fd;
	int connected;
	struct sockaddr_in server;
};

typedef struct _host host_t;

host_t *host[6];

void s_split(char ***arr, char *str, const char *del, size_t *count)
{
	size_t _count = 0;
	char buf[10010];
	char *_arr[5000];
	memset(_arr, 0, sizeof(char*) * 5000);

	strncpy(buf, str, 10010);

	char *save;
	char *s = strtok(buf, del);

	while (s != NULL)
	{
		//printf("util:%s\n", s);
		size_t __count = strlen(s);
		_arr[_count] = (char*)malloc(sizeof(char) * (__count + 1));
		memset(*(_arr + _count), 0, (__count + 1));
		strcpy(*(_arr + _count), s);
		s = strtok(NULL, del);
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

void notfound(SOCKET s)
{
	char buf[128];

	memset(buf, 0, 128);
	strcpy(buf, "HTTP/1.1 404 Not Found\r\n");
	send(s, buf, strlen(buf), 0);

	memset(buf, 0, 128);
	strcpy(buf, "Content-type: text/html\r\n\r\n");
	send(s, buf, strlen(buf), 0);

	memset(buf, 0, 128);
	strcpy(buf, "<html>\n<body>");
	send(s, buf, strlen(buf), 0);

	memset(buf, 0, 128);
	strcpy(buf, "<img src=\"http://cdn.sstatic.net/Sites/stackoverflow/img/polyglot-404.png\"></img>\n");
	send(s, buf, strlen(buf), 0);

	memset(buf, 0, 128);
	strcpy(buf, "</body>\n</html>\n");
	send(s, buf, strlen(buf), 0);
}


void html_head(SOCKET s)
{
	char buf[128];

	memset(buf, 0, 128);
	strcpy(buf, "Content-Type: text/html\n\n");
	send(s, buf, strlen(buf), 0);
	
	memset(buf, 0, 128);
	strcpy(buf, "<html>\n");
	send(s, buf, strlen(buf), 0);

	memset(buf, 0, 128);
	strcpy(buf, "<head>\n");
	send(s, buf, strlen(buf), 0);

	memset(buf, 0, 128);
	strcpy(buf, "<meta http-equiv=\"Content-Type\" content=\"text/html; charset=big5\" />\n");
	send(s, buf, strlen(buf), 0);

	memset(buf, 0, 128);
	strcpy(buf, "<title>Network Programming Homework 3</title>\n");
	send(s, buf, strlen(buf), 0);

	memset(buf, 0, 128);
	strcpy(buf, "</head>\n");
	send(s, buf, strlen(buf), 0);

	memset(buf, 0, 128);
	strcpy(buf, "<body bgcolor=#336699>\n");
	send(s, buf, strlen(buf), 0);

	memset(buf, 0, 128);
	strcpy(buf, "<font face=\"Courier New\" size=2 color=#FFFF99>\n");
	send(s, buf, strlen(buf), 0);

	memset(buf, 0, 128);
	strcpy(buf, "<table width=\"800\" border=\"1\">\n");
	send(s, buf, strlen(buf), 0);
}

void cgi_server(SOCKET s, HWND hwnd, char *qstring)
{
	char **s_array;
	size_t counter;
	int nhost = 0;

	s_split(&s_array, qstring, "&", &counter);

	//init host
	int i = 0;
	for (;i < 6; i++)
		host[i] = NULL;

	for (i = 1; i < 6; i++)
	{
		host_t *temp = (host_t*)malloc(sizeof(host_t));
		memset(temp, 0, sizeof(host_t));

		int index = i - 1;
		if (strlen(s_array[index*3]) > 3 && strlen(s_array[index*3 + 1]) > 3 && strlen(s_array[index*3 + 2]) > 3)
		{
			strcpy(temp->ip, &s_array[index*3][3]);
			temp->port = atoi(&s_array[index*3+1][3]);
			strcpy(temp->file, &s_array[index*3+2][3]);
			temp->file_fd = fopen(temp->file, "r");
			temp->connected = 0;
			host[i] = temp;
			nhost++;
		}
	
		else
			free(temp);
	}

	html_head(s);

	char sbuf[128];

	memset(sbuf, 0, 128);
	strcpy(sbuf, "<tr>\n");
	send(s, sbuf, strlen(sbuf), 0);

	for (i = 1; i < 6; i++)
	{
		if (host[i] != NULL)
		{
			memset(sbuf, 0, 128);
			strcpy(sbuf, "<td>");
			send(s, sbuf, strlen(sbuf), 0);

			memset(sbuf, 0, 128);
			sprintf(sbuf, "%s", host[i]->ip);
			send(s, sbuf, strlen(sbuf), 0);

			memset(sbuf, 0, 128);
			strcpy(sbuf, "</td>");
			send(s, sbuf, strlen(sbuf), 0);
		}
	}

	memset(sbuf, 0, 128);
	strcpy(sbuf, "</tr>\n");
	send(s, sbuf, strlen(sbuf), 0);

	memset(sbuf, 0, 128);
	strcpy(sbuf, "<tr>\n");
	send(s, sbuf, strlen(sbuf), 0);

	for (i = 1; i < 6; i++)
	{
		if (host[i] != NULL)
		{
			memset(sbuf, 0, 128);
			strcpy(sbuf, "<td");
			send(s, sbuf, strlen(sbuf), 0);

			memset(sbuf, 0, 128);
			sprintf(sbuf, " valign=\"top\" id=\"m%d\"", i-1);
			send(s, sbuf, strlen(sbuf), 0);

			memset(sbuf, 0, 128);
			strcpy(sbuf, "></td>");
			send(s, sbuf, strlen(sbuf), 0);
		}
	}
	memset(sbuf, 0, 128);
	strcpy(sbuf, "</tr>\n");
	send(s, sbuf, strlen(sbuf), 0);

	memset(sbuf, 0, 128);
	strcpy(sbuf, "</table>\n");
	send(s, sbuf, strlen(sbuf), 0);

	for (int c = 1; c < 6; c++)
	{
		if (host[c] != NULL)
		{
			SOCKET new_s = socket(AF_INET, SOCK_STREAM, 0);
			struct sockaddr_in sa;
			memset(&sa, 0, sizeof(sa));

			sa.sin_family = AF_INET;
			sa.sin_port = htons(host[c]->port);
			sa.sin_addr.s_addr = inet_addr(host[c]->ip);

			int err = WSAAsyncSelect(new_s, hwnd, CGI_SOCKET_NOTIFY, FD_CONNECT | FD_CLOSE | FD_READ | FD_WRITE);

			if (connect(new_s, (struct sockaddr *)&sa, sizeof(sa)) < 0)
			{
				if (errno != EINPROGRESS && errno != 0)
				{
					host[c] = NULL;
				}
			}

			if (host[c] != NULL)
				host[c]->sock_fd = new_s;
		}
	}
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
	LPSTR lpCmdLine, int nCmdShow)
{
	
	return DialogBox(hInstance, MAKEINTRESOURCE(ID_MAIN), NULL, MainDlgProc);
}

BOOL CALLBACK MainDlgProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam)
{
	WSADATA wsaData;

	static HWND hwndEdit;
	static SOCKET msock, ssock;
	static struct sockaddr_in sa;

	int err, r, cr;
	size_t n, qn, cn;
	SOCKET w_socket, s;
	char buf[10010] = {0}, cbuf[4096] = {0};
	char **arr, **qarr, **carr;
	string r_str, q_str;
	regex reg(".*\\.cgi.*");
	char send_buf[4096], s_buf[4096], next_cmd[4096];

	switch(Message) 
	{
		case WM_INITDIALOG:
			hwndEdit = GetDlgItem(hwnd, IDC_RESULT);
			break;
		case WM_COMMAND:
			switch(LOWORD(wParam))
			{
				case ID_LISTEN:

					WSAStartup(MAKEWORD(2, 0), &wsaData);

					//create master socket
					msock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

					if( msock == INVALID_SOCKET ) {
						EditPrintf(hwndEdit, TEXT("=== Error: create socket error ===\r\n"));
						WSACleanup();
						return TRUE;
					}

					err = WSAAsyncSelect(msock, hwnd, WM_SOCKET_NOTIFY, FD_ACCEPT | FD_CLOSE | FD_READ | FD_WRITE);

					if ( err == SOCKET_ERROR ) {
						EditPrintf(hwndEdit, TEXT("=== Error: select error ===\r\n"));
						closesocket(msock);
						WSACleanup();
						return TRUE;
					}

					//fill the address info about server
					sa.sin_family		= AF_INET;
					sa.sin_port			= htons(SERVER_PORT);
					sa.sin_addr.s_addr	= INADDR_ANY;

					//bind socket
					err = bind(msock, (LPSOCKADDR)&sa, sizeof(struct sockaddr));

					if( err == SOCKET_ERROR ) {
						EditPrintf(hwndEdit, TEXT("=== Error: binding error ===\r\n"));
						WSACleanup();
						return FALSE;
					}

					err = listen(msock, 2);
		
					if( err == SOCKET_ERROR ) {
						EditPrintf(hwndEdit, TEXT("=== Error: listen error ===\r\n"));
						WSACleanup();
						return FALSE;
					}
					else {
						EditPrintf(hwndEdit, TEXT("=== Server START ===\r\n"));
					}

					break;
				case ID_EXIT:
					EndDialog(hwnd, 0);
					break;
			};
			break;

		case WM_CLOSE:
			EndDialog(hwnd, 0);
			break;

		case CGI_SOCKET_NOTIFY:
			switch( WSAGETSELECTEVENT(lParam) )
			{
				case FD_CONNECT:
					for (int c = 1; c < 6; c++)
					{
						if (host[c] != NULL && host[c]->sock_fd == wParam)
							host[c]->connected = 1;
					}
					break;

				case FD_READ:
					for (int c = 1; c < 6; c++)
					{
						if (host[c] != NULL && host[c]->connected == 1)
						{
							s = host[c]->sock_fd;
							memset(cbuf, 0, 4096);

							cr = recv(s, cbuf, 4096, 0);
							if (cr <= 0) continue;

							s_split(&carr, cbuf, "\n", &cn);

							//send per line
							for (int i = 0; i < cn; i++)
							{
								memset(s_buf, 0, 4096);
								strcpy(s_buf, carr[i]);

								if (strncmp(s_buf, "% ", 2) == 0)
								{
									memset(next_cmd, 0, 4096);
									fgets(next_cmd, 4096, host[c]->file_fd);
									send(s, next_cmd, strlen(next_cmd), 0);
									replace_html(next_cmd);

									if (strncmp(next_cmd, "exit", 4) == 0)
									{
										closesocket(s);
										host[c] = NULL;
									}

									for (list<SOCKET>::iterator it = Socks.begin(); it != Socks.end(); ++it)
									{
										memset(s_buf, 0, 4096);
										sprintf(s_buf, "<script>document.all['m%d'].innerHTML += \"%% <b>%s</b>\";</script>\n", c - 1, next_cmd);
										send(*it, s_buf, strlen(s_buf), 0);
									}
								}

								else
								{
									for (list<SOCKET>::iterator it = Socks.begin(); it != Socks.end(); ++it)
									{
										memset(s_buf, 0, 4096);
										sprintf(s_buf, "<script>document.all['m%d'].innerHTML += \"%s<br>\";</script>\n", c - 1, next_cmd);
										send(*it, s_buf, strlen(s_buf), 0);
									}
								}
							}
						}
					}
			}
			EditPrintf(hwndEdit, TEXT("aaaaaaaaaaaaaaaaaa\n"));
			break;
		case WM_SOCKET_NOTIFY:
			switch( WSAGETSELECTEVENT(lParam) )
			{
				case FD_ACCEPT:
					ssock = accept(msock, NULL, NULL);
					Socks.push_back(ssock);
					EditPrintf(hwndEdit, TEXT("=== Accept one new client(%d), List size:%d ===\r\n"), ssock, Socks.size());
					break;
				case FD_READ:
				//Write your code for read event here.
					w_socket = wParam;
					r = recv(w_socket, buf, 10010, 0);
					EditPrintf(hwndEdit, TEXT("recv:\t%s\n"), buf);
					if (r == 0 || (r == SOCKET_ERROR && WSAGetLastError() == WSAEWOULDBLOCK))
					{
						EditPrintf(hwndEdit, TEXT("errno:\t%d\n"), r);
						return FALSE;
					}

					s_split(&arr, buf, " ", &n);
					if (n < 3)
						return false;

					EditPrintf(hwndEdit, TEXT("para:\t%s\n"), arr[1]);

					//exe cgi
					r_str = arr[1];
					
					if (strstr(arr[1], "cgi") != NULL)
					{
						s_split(&qarr, &arr[1][1], "?", &qn);
						if (qn != 2)
							return false;
						EditPrintf(hwndEdit, TEXT("query:\t%s\n"), qarr[1]);
						cgi_server(w_socket, hwnd, qarr[1]);
					}

					//html file
					else
					{
						FILE* html_file = fopen(&arr[1][1], "r");
						if (html_file == NULL)
						{
							notfound(w_socket);
							closesocket(w_socket);
						}

						else
						{
							memset(send_buf, 0, 4096);
							strcpy(send_buf, "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n");
							send(w_socket, send_buf, strlen(send_buf), 0);
							while (fgets(send_buf, 4096, html_file) != 0)
								send(w_socket, send_buf, strlen(send_buf), 0);

							closesocket(w_socket);
						}
					}
					break;
				case FD_WRITE:
				//Write your code for write event here

					break;
				case FD_CLOSE:
					break;
			};
			break;
		
		default:
			return FALSE;


	};

	return TRUE;
}

int EditPrintf (HWND hwndEdit, TCHAR * szFormat, ...)
{
     TCHAR   szBuffer [1024] ;
     va_list pArgList ;

     va_start (pArgList, szFormat) ;
     wvsprintf (szBuffer, szFormat, pArgList) ;
     va_end (pArgList) ;

     SendMessage (hwndEdit, EM_SETSEL, (WPARAM) -1, (LPARAM) -1) ;
     SendMessage (hwndEdit, EM_REPLACESEL, FALSE, (LPARAM) szBuffer) ;
     SendMessage (hwndEdit, EM_SCROLLCARET, 0, 0) ;
	 return SendMessage(hwndEdit, EM_GETLINECOUNT, 0, 0); 
}