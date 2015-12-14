//
// GetHTTP.cpp
//
// Retrieves a file using the Hyper Text Transfer Protocol
// and validate its header.
//
// author:xia wu
// date: 09/28/2006
//
// usage:
//		GetHTTP -f test.check
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <io.h>
#include <winsock.h>
#include <process.h>

#define MAX_LINE_LEN 4096
#define MAX_VOLUME 4096
//TAG definition
#define TAG_EXIST "exist"
#define TAG_NO_EXIST "no_exist"
#define TAG_DOWNLOAD "download"
#define TAG_PROXY_ADDR "proxy_address"
#define TAG_PROXY_PORT "proxy_port"
#define TAG_DENY_PAGE "deny_page"
#define TAG_PROXY_SWITCH "proxy_switch"
#define TAG_EXIST_PAGE "exist_page"
#define TAG_NO_EXIST_PAGE "no_exist_page"
#define TAG_EXIST_DENY "exist_deny"
//HTTP Response Status
#define HTTP_STATUS_OK 1
#define HTTP_STATUS_DENY_PAGE -1
#define HTTP_STATUS_NOT_FOUND -2
//HTTP Response string
#define HTTP_HEADER_OK_1_1 "HTTP/1.1 200 OK"
#define HTTP_HEADER_OK_1_0 "HTTP/1.0 200 OK"
//Switch variable
#define PMI_ENABLE 1
//status machine
#define STATUS_DOWNLOAD 3
#define STATUS_EXIST 2
#define STATUS_NO_EXIST 1
#define STATUS_EXIST_DENY 4
//file content length
#define MAX_FILE_LEN 1024
//debug status
#define TEST_DEBUG 0
//proxy switch
#define PROXY_ON "On"
#define PROXY_OFF "Off"

#define DEFAULT_DOWNLOAD_DIRECTORY "downloads"
//HTTP Prefix
#define PREFIX_HTTP "http://"
//
//structure definition
//
struct Line
{
	char data[MAX_LINE_LEN];
};

//
//global variable definiton
//
static int currentLine = 0;
struct Line lines[MAX_VOLUME];
struct Line tmp_lines[MAX_VOLUME];
static char CacheHost[MAX_LINE_LEN];
static char CacheFile[MAX_LINE_LEN];
static char CacheEncode[MAX_LINE_LEN];
static char CacheConvert[MAX_LINE_LEN];
static char proxy_addr[MAX_LINE_LEN];
static int  proxy_port = 4433;
static char fileName[126] = "test.check";
static int gStatus = STATUS_EXIST;
static int gProxyEnable = 0;
static int gExistPageEnable = 0;
static int gNoExistPageEnable = 0;
static char szBuffer[MAX_FILE_LEN];
//
//function defintion
//
void GetHTTP(LPCSTR lpServerName, LPCSTR lpFileName, LPCSTR lpExpected);
int ReadINIFile(LPCSTR lpFileName, LPCSTR lpSectionName);
char *Trim(char *p);
char *ParseHost(char *p);
char *ParseFile(char *p);
char *Encode(char *p);
char *ToChar(char src);
int ValidateHeader(char *header);
void DownLoad(char *lpContent, const char *lpFileName, int len);
void FilterHTTPHeader(char *lpContent);
void ParseFileName(const char *lpPath, char *lpFileName);
int GetProxyEnable();
int CompareExistHeader(const char *header, struct Line *lines, int len);
char* lowerstr(char *p);
// Helper macro for displaying errors
#define PRINTERROR(s)	\
		fprintf(stderr,"\n%: %d\n", s, WSAGetLastError())

////////////////////////////////////////////////////////////
void main(int argc, char **argv)
{
	WORD wVersionRequested = MAKEWORD(1,1);
	WSADATA wsaData;
	
	int nRet;
	int i = 1;
	//
	//parse args
	//

	while (i <= argc - 1)
	{
		if (strcmp(argv[i], "-f") == 0)
		{
			if (i + 1 > argc - 1)
			{
				printf("format error\n");
				break;
			}
			strcpy(fileName, argv[i + 1]);
			printf("get filename:%s", fileName);
			i += 2;
			if (i > argc)
			{
				break;
			}
			continue;
		}
		
		if (strcmp(argv[i], "-h") == 0)
		{
			printf("This a utility of validate the http resource.\n");
			printf("usage:\n");
			printf("-f filename \t the location of ini file\n");
			printf("-h          \t help of the utility\n");
			printf("for example:\n");
			printf("gethttp -f test.check\n");
			return;
		}
		i++;
	}

	//
	// Initialize WinSock.dll
	//
	nRet = WSAStartup(wVersionRequested, &wsaData);
	if (nRet)
	{
		fprintf(stderr,"\nWSAStartup(): %d\n", nRet);
		WSACleanup();
		return;
	}
	
	//
	// Check WinSock version
	//
	if (wsaData.wVersion != wVersionRequested)
	{
		fprintf(stderr,"\nWinSock version not supported\n");
		WSACleanup();
		return;
	}
	//
	//read the proxy address and port from *.ini file
	//
	gProxyEnable = GetProxyEnable(); 
	if (gProxyEnable)
	{
		ReadINIFile(fileName, TAG_PROXY_ADDR);
		if (currentLine > 0)
		{
			strcpy(proxy_addr, Trim(lines[0].data));
		}
		else
		{
			printf("the address of proxy server is not setting correctly.\n");
		
			return;
		}
	
		ReadINIFile(fileName, TAG_PROXY_PORT);
		if (currentLine > 0)
		{
			proxy_port = atoi(Trim(lines[0].data));
		}
		else
		{
			printf("the address of proxy server is not setting correctly.\n");
		
			return;
		}
	}

	//
	// Set "stdout" to binary mode
	// so that redirection will work
	// for .gif and .jpg files
	//
	_setmode(_fileno(stdout), _O_BINARY);

	//exist section
	ReadINIFile(fileName, TAG_EXIST);
	//printf("currentline is %d\n", currentLine);
	printf("================Exist==============\n");
	//printf("currentLine is %d\n", currentLine);
	int totalLine = currentLine;
	

	gStatus = STATUS_EXIST;
	for (i = 0; i < totalLine; i++)
	{
		tmp_lines[i] = lines[i];
	}
	for (i = 0; i < totalLine; i++)
	{
		GetHTTP(ParseHost(Trim(tmp_lines[i].data)), ParseFile(Trim(tmp_lines[i].data)), "Exist");
	}

	//no_exist section
	gStatus = STATUS_NO_EXIST;
	ReadINIFile(fileName, TAG_NO_EXIST);
	totalLine = currentLine;
	
	printf("================NO Exist==============\n");
	for (i = 0; i < totalLine; i++)
	{

		tmp_lines[i] = lines[i];
	}

	for (i = 0; i < totalLine; i++)
	{

		
		GetHTTP(ParseHost(Trim(tmp_lines[i].data)), ParseFile(Trim(tmp_lines[i].data)), "No Exist");
	}

	//exist_deny section
	gStatus = STATUS_EXIST_DENY;
	ReadINIFile(fileName, TAG_EXIST_DENY);
	totalLine = currentLine;
	
	printf("================Exist_Deny==============\n");
	for (i = 0; i < totalLine; i++)
	{

		tmp_lines[i] = lines[i];
	}

	for (i = 0; i < totalLine; i++)
	{
		GetHTTP(ParseHost(Trim(tmp_lines[i].data)), ParseFile(Trim(tmp_lines[i].data)), "Exist but Deny");
	}

	//download section
	gStatus = STATUS_DOWNLOAD;
	ReadINIFile(fileName, TAG_DOWNLOAD);
	
	totalLine = currentLine;

	printf("================Download==============\n");
	for (i = 0; i < totalLine; i++)
	{
		tmp_lines[i] = lines[i];
	}

	for (i = 0; i < totalLine; i++)
	{

		GetHTTP(ParseHost(Trim(tmp_lines[i].data)), ParseFile(Trim(tmp_lines[i].data)), "Exist");
	}
	//
	// Release WinSock
	//
	WSACleanup();

	while (1);
}

////////////////////////////////////////////////////////////

void GetHTTP(LPCSTR lpServerName, LPCSTR lpFileName, LPCSTR lpExpected)
{
	//
	// Use inet_addr() to determine if we're dealing with a name
	// or an address
	//
	//printf("server is %s, file is %s\n", lpServerName, lpFileName);
	IN_ADDR		iaHost;
	LPHOSTENT	lpHostEntry;
	char buffer[128];
	strcpy(buffer, lpServerName);
	char *p = buffer;
	int port = 80;


	if (gProxyEnable)
	{
		iaHost.s_addr = inet_addr(proxy_addr);
	}
	else
	{

		if (strstr(buffer, ":") != NULL)
		{
			while (*p != ':')p++;
			*p = '\0';
			p++;
			port = atoi(p);
			//printf("get port:%d\n", port);
			//printf("buffer is %s\n", buffer);
			iaHost.s_addr = inet_addr(buffer);
		}
		else
			iaHost.s_addr = inet_addr(buffer);
	}
	
	//printf("buffer is %s\n", buffer);

	/*if (iaHost.s_addr == INADDR_NONE)
	{
		// Wasn't an IP address string, assume it is a name
			//lpHostEntry = gethostbyname(lpServerName);
		lpHostEntry = gethostbyname(buffer);
		//printf("addr is %d\n", iaHost.s_addr);
	
	}
	else
	{	
			// It was a valid IP address string
		//printf(" another addr is %d\n", iaHost.s_addr);
			//lpHostEntry = gethostbyaddr((const char *)&iaHost, 
					//	sizeof(struct in_addr), AF_INET);
		lpHostEntry = gethostbyname(buffer);
	}
	*/
	if (gProxyEnable)
		lpHostEntry = gethostbyname(proxy_addr);
	else
		lpHostEntry = gethostbyname(buffer);



	if (lpHostEntry == NULL)
	{
		printf("not found host %s\n", proxy_addr);
		return;
	}
	
	//	
	// Create a TCP/IP stream socket
	//
	SOCKET	Socket;	

	Socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (Socket == INVALID_SOCKET)
	{
		printf("create socket error\n"); 
		return;
	}


	//
	// Find the port number for the HTTP service on TCP
	//
	LPSERVENT lpServEnt;
	SOCKADDR_IN saServer;

	//lpServEnt = getservbyname("http", "tcp");
	//if (lpServEnt == NULL)
	//{
	//saServer.sin_port = htons(port);
	if (gProxyEnable)
		saServer.sin_port = htons(proxy_port);
	else
		saServer.sin_port = htons(port);
	//	printf("lpServEnt is null");
	//}
	//else
	//{
	//	saServer.sin_port = lpServEnt->s_port;
	//	printf("lpServEnt is not null, port is %d", lpServEnt->s_port);
	//}


	//
	// Fill in the rest of the server address structure
	//
	saServer.sin_family = AF_INET;
	saServer.sin_addr = *((LPIN_ADDR)*lpHostEntry->h_addr_list);


	//
	// Connect the socket
	//
	int nRet;

	nRet = connect(Socket, (LPSOCKADDR)&saServer, sizeof(SOCKADDR_IN));
	if (nRet == SOCKET_ERROR)
	{
		printf("URL:%s%s\n", lpServerName, lpFileName);
		printf("Host:%s is not avaliable.\n\n", lpServerName);
		closesocket(Socket);
		return;
	}

	
	//
	// Format the HTTP request
	//
	
	sprintf(szBuffer, "GET http://%s%s HTTP/1.0\r\nHost: %s\r\nAccept: */* \r\n\r\n", lpServerName,lpFileName, lpServerName);
	//p = Encode(szBuffer);
	//printf("szBuffer size is %d\n", strlen(szBuffer));
	//printf("Encode size is %d\n", strlen(p));
	p = NULL;
	p = szBuffer;
	
	if (TEST_DEBUG)
	{
		printf("send data is %s\n", szBuffer);
	}
	nRet = send(Socket, p, strlen(p), 0);
	if (nRet == SOCKET_ERROR)
	{
		printf("send eror\n");
		closesocket(Socket);	
		return;
	}

	// Wait to receive, nRet = NumberOfBytesReceived
	nRet = recv(Socket, szBuffer, MAX_FILE_LEN, 0);
	if (nRet == SOCKET_ERROR)
	{
		printf("receive error\n");
		return;
	}
	
	// Did the server close the connection?
	if (nRet == 0)
	{
		printf("connection is refused!\n");
		return;
	}
	szBuffer[nRet] = '\0';
	
	//printf("receive data:%s\n", szBuffer);
	printf("URL     :%s%s\n", lpServerName, lpFileName);
	printf("Expected:%s\n", lpExpected);
	
	//
	//here, more reponse headers will be supported in future.
	//
	int status = 0; 
	status = ValidateHeader(szBuffer); 

	if ( status == HTTP_STATUS_OK)
	{
		printf("Result  :Exist\n");
		//download file
		if (gStatus == STATUS_DOWNLOAD)
		{
			//filter the response header
			FilterHTTPHeader(szBuffer);
			do 
			{
				szBuffer[nRet] = '\0';
				DownLoad(szBuffer, lpFileName, strlen(szBuffer));
			}
			while((nRet = recv(Socket, szBuffer, MAX_FILE_LEN, 0)) > 0);
		}
	}
	else if (status == HTTP_STATUS_DENY_PAGE)
	{
		printf("Result  :Exist but Deny\n");
	}
	else
		printf("Result  :NO Exist\n");
	printf("\n");

	closesocket(Socket);
	
}

//
//read the *.ini file
//
//return -1 if exception
//return 1 if success
//
int ReadINIFile(LPCSTR lpFileName, LPCSTR lpSectionName)
{
	FILE *fp = NULL;
	static char buffer[MAX_LINE_LEN];
	static char key[MAX_LINE_LEN];

	*buffer = '\0';
	*key = '\0';
	currentLine = 0;
	if ((fp = fopen(lpFileName, "r")) == NULL)
	{
		printf("Error:open file\n");

		return -1;
	}
	
	//find section
	strcpy(key, "[");
	strcat(key, lpSectionName);
	strcat(key, "]");
	
	while (!(feof(fp)))
	{
		if ((fgets(buffer, MAX_LINE_LEN, fp)) != NULL)
		{
			//find section 
			if (strncmp(buffer, key, strlen(key)) == 0)
			{
				while (!feof(fp))
				{
					if (fgets(buffer, MAX_LINE_LEN, fp) != NULL)
					{
						if (*buffer == '#')
						{
							continue;
						}
						if (strlen(Trim(buffer)) <= 0)
						{
							continue;
						}
						if (*Trim(buffer) != '[')
						{
							strcpy(lines[currentLine].data, Trim(buffer));
							currentLine++;
						}
						else
						{
							break;
						}
					}
				}
			}
		}
	}
	return 1;
}

char *Trim(char *p)
{
	char *q = p;
	char *r = NULL;
	int len = 0;
	while ((*q == ' ' || *q == '\t') && *q != '\n')q++;
	
	len = strlen(q);
	if (len > 0)
	{
		r = q + len - 1;
		while (*r == '\t' || *r == '\n' || *r == '\r' || *r == ' '){
			*r = '\0';
			r--;
		}
	}
	else
	{
		return NULL;
	}

	return q;
}

char *ParseHost(char *p)
{
	char *q = NULL;
	char *buffer = CacheHost;

	strcpy(buffer, " ");
	if ((q = strstr(p, "http://")) != NULL)
	{
		q += 7;
		while (*q != '/')
		{
			*buffer = *q;
			buffer++;
			q++;
		}
		*buffer = '\0';
		return CacheHost;
	}
	return NULL;
}
char *ParseFile(char *p)
{
	char *q = NULL;
	char *buffer = CacheFile;
	int len = 0;
	strcpy(buffer, " ");
	if ((q = strstr(p, PREFIX_HTTP)) != NULL)
	{
		q += strlen(PREFIX_HTTP);
		while (*q != '/')
			q++;
		if (*q == '/')
		{
			strcpy(buffer, q);
		}
		//printf("parse file:%s\n", buffer);
		
		return CacheFile;
	}
	return NULL;
}
//
//enocde string for url
//
char *Encode(char *p)
{
	char *q = p;
	*CacheEncode = '\0';
	char *r = CacheEncode;

	while (*q)
	{
		if ((*(unsigned char*)q) > 127)
		{
			*r = '%';
			r++;
			strcat(r, ToChar(*q));

			r += 2;
			*r = '%';
		 
			r++;
			strcat(r, ToChar(*(q + 1)));
			 
			r += 2;
			q += 2;
		}
		else
		{
			*r = *q;
			q++;
			r++;
		}
	}
	*r = '\0';
	
	//printf("encode is %s\n", CacheEncode);

	return CacheEncode;
}

//
//convert hex value of a char to string
//
char *ToChar(char c)
{
	*CacheConvert = '\0';
	sprintf(CacheConvert, "%x", (unsigned char)c);
	*(CacheConvert + 2) = '\0';
	//printf("ToChar is %s\n", CacheConvert);

	return CacheConvert;
}
//
//validate the header of http response
//return 1 if http respose is OK
//
int ValidateHeader(char *header)
{
	
	int totalLine;
	//read the exist_page section
	ReadINIFile(fileName, TAG_EXIST_PAGE);
	totalLine = currentLine;
	if (totalLine > 0)
	{
		gExistPageEnable = 1;
	}
	#define MAX_ARRAY_SIZE 128
	struct Line tmpLines[MAX_ARRAY_SIZE];
	

	for (int i = 0; i < totalLine && i < MAX_ARRAY_SIZE; i++)
	{
		strcpy(tmpLines[i].data,lines[i].data);
	}

	if (CompareExistHeader(header, tmpLines, totalLine) == 0)
	{
		//only for pmi specially
		if (PMI_ENABLE)
		{
			ReadINIFile(fileName, TAG_DENY_PAGE);
			if (TEST_DEBUG)
			{
				printf("read deny page line is %d\n", currentLine);
			}

			totalLine = currentLine;
			for (int i = 0; i < totalLine; i++)
			{
				if (strstr(header, Trim(lines[i].data)) == NULL)
				{
					if (TEST_DEBUG)
					{
						printf("not found deny page string:%s\n", lines[i].data);
					}
					return HTTP_STATUS_OK;
				}
			}
			if (totalLine > 0)
			{
				if (TEST_DEBUG)
				{
					printf("found deny page.\n");
				}
				return HTTP_STATUS_DENY_PAGE;
			}
			
		}
		else
			return HTTP_STATUS_OK;
	}

	return HTTP_STATUS_NOT_FOUND;
}

//
//
//
int CompareExistHeader(const char *header, struct Line *lines, int len)
{
	if (gExistPageEnable == 0)
	{
		if (strncmp(header, HTTP_HEADER_OK_1_1, 15))
		{
			return 0;
		} 
		if (strncmp(header, HTTP_HEADER_OK_1_0, 15))
		{
			return 0;
		}
	}

	for (int i = 0; i < len; i++)
	{
		if (strncmp(header, Trim(lines[i].data), strlen(Trim(lines[i].data))) == 0)
		{
			return 0;
		}
	}
	
	return 1;
}
//
//download file by url
//
void DownLoad(char *lpContent, const char *lpFileName, int len)
{
	if (TEST_DEBUG)
	{
		printf("starting download\n");
	}

	FILE *fp = NULL;
	char path[256];
	char fileName[256];
	strcpy(path, DEFAULT_DOWNLOAD_DIRECTORY);
	//create directory
	CreateDirectory(DEFAULT_DOWNLOAD_DIRECTORY, NULL);

	ParseFileName(lpFileName, fileName);
	
	strcat(path, "/");
	strcat(path,fileName);
	
	if (TEST_DEBUG)
	{
		printf("download file path:%s\n", path);
	}

	if ((fp = fopen(path, "a+")) == NULL)
	{
		printf("create file error!");
		return;
	}
	//fseek(fp, 0, SEEK_END);
	fwrite(lpContent, 1, len, fp);
	fclose(fp);
}
//
//
//
void FilterHTTPHeader(char *lpContent)
{
	char *p = lpContent;
	char *q = NULL;
	int len = 0;
	q = strstr(p, "\r\n\r\n");
	
	if (q == NULL)
	{
		printf("received exception HTTP header.\n");
		return;
	}
	q = q+4;
	len = strlen(q);
	strcpy(lpContent, q);
	lpContent[len] = '\0';
	
}

//
//parse file name from file path
//note that only for windows directory format and separator is '/'
//
void ParseFileName(const char *lpPath, char *lpFileName)
{
	const char *p = lpPath;
	p = p + strlen(lpPath) - 1;

	while (*p != '/')
	{
		p--;
	}
	p++;

	strcpy(lpFileName, p);
}
//
//return the status of proxy
//
int GetProxyEnable()
{
	ReadINIFile(fileName, TAG_PROXY_SWITCH);
	if (currentLine > 0)
	{
		struct Line tmp;
		strcpy(tmp.data, lines[0].data);
		if (strcmp(Trim(tmp.data), PROXY_ON) == 0)
		{
			return 1;
		}
	}
	return 0;
}

char* lowerstr(char *p)
{
  int len = strlen(p);
  int i = 0;
  for (i = 0; i < len; i++)
    {
	  p[i] = tolower(p[i]);
    }
  return p;
}