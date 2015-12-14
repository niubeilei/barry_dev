#include <stdlib.h> 
#include <readline/readline.h>
#include <readline/history.h>
#include <strings.h>
#include <sys/types.h>  
#include <stdio.h>  
#include <sys/socket.h>  
#include <string.h>  
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>


int main()  
{  
	int cfd; 
	int recbytes;  
	int sin_size;  
	char buffer[409600]={0};/* Receive buffer */  
	struct sockaddr_in s_add; /* To the storage service client and the IP, port and other information structure */  
	unsigned short portnum=10000;  /* Server using a communication port, can be changed, must be the same and > server */  

	printf("Hello,welcome to client !\r\n");
	/* To establish a socket using the Internet, TCP streaming */  
	cfd = socket(AF_INET, SOCK_STREAM, 0);  
	if(-1 == cfd)  
	{  
		printf("socket fail ! \r\n");  
		return -1;  
	}  
	printf("socket ok !\r\n");  
	/* The server IP and port information */  
	bzero(&s_add,sizeof(struct sockaddr_in));  
	s_add.sin_family=AF_INET;  
	s_add.sin_addr.s_addr= htonl(INADDR_ANY); 
	s_add.sin_port=htons(portnum);  

	/* Client connect to the server, the parameters of the socket file descriptor, address information, and address the structure size */  
	if(-1 == connect(cfd,(struct sockaddr *)(&s_add), sizeof(struct sockaddr)))  
	{  
		printf("connect fail !\r\n"); 
		return -1;  
	}  
	printf("connect ok !\r\n");
/*    printf("Please enter the need to the number of lines:");
	char need_num_str[100];
	scanf("%s", need_num_str);
	int send_size = -1;
	if ((send_size = send(cfd, need_num_str, sizeof(need_num_str), 0)) > 0)
	{
		//printf("send OK!!!! %d\n", send_size);
		printf("recv OK!!!!\n");
	}*/
	/*The connection is successful, from the server to receive characters*/ 
	bzero(buffer, sizeof(buffer));
	while ( (recbytes = recv(cfd, buffer, sizeof(buffer) - 1, 0)) > 0)
	{
		buffer[recbytes]='\0';
		printf("have received:\n%s\n", buffer);
		bzero(buffer,sizeof(buffer));
	}
	getchar();   
	close(cfd); /* Close the connection, the communication is complete */  
	return 0;  
} 
