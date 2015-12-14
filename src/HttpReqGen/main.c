#include "HttpReqGen.h"
#include "RandomUtil.h"
#include "main.h"

int main(int argc, char **argv)
{

	if (argc != 2 && (argv[1] != "urls" || argv[1] != "domains")) {
		printf("Usage:<domains urls>\n");
		return 0;
	}

	if (!strcmp(argv[1], "urls")) 
		gen_request_urls();
	else if (!strcmp(argv[1], "domains"))
		gen_request_domains(); 

	return 0;
}

int gen_request_urls()
{
	char request[1024], url[256], reqline[1024], header[1024], host[128],method[10],version[20];
	static char *header_element, *uri;
	FILE *fd;
	int i = 0;
	pthread_t pid;
	AosRandInitialize(0);
	fd = fopen(URLS_PATH, "r");
	while ( fgets(url, sizeof(url), fd) != NULL) { 
		for(i = 0; i<sizeof(url); i++){
			if (url[i] == '\n') {
				url[i] = '\0';
				break;
			}
		}
		for (i = 0; i < sizeof(url); i++) {
			if (url[i] == '/')
				break;
		}
		uri = url + i;
		strcpy(host, (char*)strtok(url, "/"));
		gen_method(method);
		gen_version(version);
		header_element = gen_header_element();
		sprintf(reqline, "%s %s %s\r\n", method, uri, version);
		sprintf(header, "HOST: %s\r\n%s", host, header_element);
//		AosRandHttpRequest_CreateBody(body, 0);
		sprintf(request, "%s%s\r\n\r\n", reqline, header);

		example.host = host;
		example.message = request;
		for(i = 0; i< 1000; i++);
		for (i = 0; i < MAX_THREAD; i++)
			pthread_create(&pid, NULL, &sendmessage, &example);
	}
	sleep(3600);
	return 0;
}	

int gen_request_domains() 
{
	char request[1024], header[1024], reqline[1024], host[128];
	char* header_element;
	FILE *fd;
	int i = 0;
	pthread_t pid;

	AosRandInitialize(0);

	fd = fopen(DOMAINS_PATH, "r");
	while (fgets(host, 128, fd) != NULL) { 
		for(i = 0; i<sizeof(host); i++) {
			if (host[i] == '\n') {
				host[i] = '\0';
				break;
			}
		}
		memset(request,0,sizeof(request));
		header_element = gen_header_element();
		AosRandHttpRequest_CreateReqLine(reqline, 1024);
		sprintf(header, "HOST: %s\r\n%s", host, header_element);
//		AosRandHttpRequest_CreateBody(body, 0);
		sprintf(request, "%s%s\r\n\r\n", reqline, header);

		example.host = host;
		example.message = request;
		for (i =0; i < MAX_THREAD; i++)
			pthread_create(&pid, NULL, &sendmessage, &example);
	}
	sleep(3600);
	return 0;
}
	
static void* sendmessage(struct thread_struct *example)
{
	struct sockaddr_in	servaddr;
	struct hostent *host_struct;
	static char readbuf[1024];
	fd_set rset;
	char **host_ip_addr;
	char str[256];
	char *ip, *tmp, *ipaddr, *buf;
	int flags, i = 0;
	int nwritten = 0; 
	int sockfd, nleft = sizeof(buf);

	ipaddr = example->host;
	buf = example->message;
	tmp = buf;
	if(!is_ipaddr(ipaddr)) {
		while (1) {
			if (NULL != (host_struct =  gethostbyname(ipaddr)))
				break;
			i++;
			if (5 == i)
				return 0;
		}
		host_ip_addr = host_struct->h_addr_list;	
		ip = inet_ntop(AF_INET, *host_ip_addr, str, sizeof(str));
	}
	else {
		ip = ipaddr;
	}
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	
	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(80);
	inet_pton(AF_INET, ip, &servaddr.sin_addr);

	if (connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
		printf("Connect Error");
		printf("%d\n",errno);
		return -1;
	}
	while( nleft >0)
	{
		if ( (nwritten = write(sockfd,tmp,nleft)) <= 0) {
			if (errno == EINTR) {
				nwritten = 0;
			}
			else
				return -1;
		}
		nleft -= nwritten;
		tmp += nwritten;
	}	
	write(sockfd,"\r\n",sizeof("\r\n"));

	memset(readbuf,0,sizeof(readbuf));
	FD_ZERO(&rset);
	FD_SET(sockfd, &rset);

	if (select(sockfd+1, &rset, NULL, NULL, NULL) == 0) {
		close(sockfd);			
		return 0;
	}

	if (read(sockfd, readbuf, sizeof(readbuf)) < 0) {
		printf("Read Error\n");
		return -1;
	}
	printf("-------------------------------------------------------------------------\n");
	printf("%s\n",readbuf);
	close(sockfd);
	return 0;
}

int is_ipaddr(char *buf)
{
	int i;
	for (i = 0; i < sizeof(buf); i++) {
		if ((buf[i] - 'a') >= 0)
			return 0;
	}
	return 1;
}
