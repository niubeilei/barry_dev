#include<stdlib.h>
#include<dirent.h>
#include<sys/stat.h>
#include<dirent.h>
#include <sys/types.h>  
#include <stdio.h>  
#include <sys/socket.h>  
#include <string.h>
#include <arpa/inet.h>
#include <time.h>
#include <unistd.h>
#include <netinet/in.h>
int main(int argc,char** argv)
{
	if (argc != 5) {
		printf("Usage: %s <total_lines> <1|2> <t> <lines>\n", argv[0]);
		printf(" total_lines - A total of data rows\n");
		printf("           1 - create stat table\n");
		printf("           2 - create join table\n");
		printf("           t - Send time interval\n ");
		printf("      lines - Send the number of rows at a time\n");
		return -1;
	}
	long long n = atoll(argv[1]);
	long long m = atoll(argv[4]);
	long long num;
	int table = atoi(argv[2]);
	int T= atoll(argv[3]);
	char buff[204800];
	char bigBuff[409600];
	buff[0] = 0;
	char* data = buff;
	const char* f1 = "\"%d\",";
	const char* prefix1 = "f";
	const char* prefix2 = "col";
	char *prefix;
	if (table == 1)
		prefix = (char *)prefix1;
	else
		prefix = (char *)prefix2;
	for (int i = 0; i < 3; i++) {
		data = strcat(data, f1);
	}
	for (int i = 4; i <= 10; i++) {
		sprintf(data, "%s\"%s%d_%%d\"", data, prefix, i);
		if (i == 10)
			data = strcat(data, "\n");
		else
			data = strcat(data, ",");
	}

/*	int dateLenArray[10000];
	if(n > 0)
	{
		srand( (unsigned)time( NULL ) );
		//generate 100 lines of records and save into bigBuff
		for (long long i=0; i<n; i++) {

			if (table == 1)
				num = rand()%20;
			else
				num = i;

			int prevLen= strlen(bigBuff);
			sprintf(bigBuff+prevLen, buff, 
					num,
					rand()%100,
					rand()%5,
					rand()%1000,
					rand()%1000,
					rand()%1000,
					rand()%1000,
					rand()%1000,
					rand()%1000,
					rand()%1000
				   );
			int currentLen = strlen(bigBuff);
			dateLenArray[i] = currentLen;

			printf("prevLen: %d \t currentLen: %d\n", prevLen, currentLen);
			printf("%s\n", bigBuff);
		//	send(sfp, bigBuff, strlen(bigBuff),0);
			if(i%1==0){
				sleep(T);
			}


		}
	}*/
int sfp,nfp; /* Define two descriptors */ 
struct sockaddr_in s_add,c_add;  
int recbytes;
unsigned int sin_size;  
unsigned short portnum=10000; /* Server using the port */ 

printf("Hello,welcome to my server !\r\n");
sfp = socket(AF_INET, SOCK_STREAM, 0);
if(sfp == -1)  
{  
	printf("socket fail ! \r\n");  
	return -1;  
}  
printf("socket ok !\r\n"); 
/* Fill the server port address information, in order to use the below address and port to monitor */  
bzero(&s_add,sizeof(struct sockaddr_in));  
s_add.sin_family=AF_INET;  
s_add.sin_addr.s_addr=htonl(INADDR_ANY); /* Here with full address 0, that is, all */  
s_add.sin_port=htons(portnum);  
/* Use the bind to bind port */  
if(-1 == bind(sfp,(struct sockaddr *)(&s_add), sizeof(struct sockaddr)))  
{  
	printf("bind fail !\r\n");  
	return -1;  
}  
printf("bind ok !\r\n");  
/* To start listening to the corresponding port */  
if(-1 == listen(sfp,5))  
{  
	printf("listen fail !\r\n");  
	return -1;  
}  
printf("listen ok\r\n");

while(1)  
{  
	sin_size = sizeof(struct sockaddr_in); 
	nfp = accept(sfp, (struct sockaddr *)(&c_add), &sin_size);  
	if(-1 == nfp)  
	{  
		printf("accept fail !\r\n");  
		return -1;  
	}
	printf("accept ok!\r\nServer start get connect from %#x : %#x\r\n",ntohl(c_add.sin_addr.s_addr),ntohs(c_add.sin_port)    ); 


/*	int s=recv(nfp,buff, sizeof(buff)-1,0);
	int value = atoi(buff);
	char ss[1024];
	int size = dateLenArray[value-1];
	strncpy(ss, bigBuff, size);
	if(-1 == send(nfp, ss, size,0)){
		printf("send data fail !\r\n"); 
		return -1;

	}*/
//	printf("have sent: \n%s\n",ss);
	int dateLenArray[10000];
	if(n > 0)
	{
		srand( (unsigned)time( NULL ) );
		//generate 100 lines of records and save into bigBuff
		for (long long i=0; i<n; i++) {

			if (table == 1)
				num = rand()%20;
			else
				num = i;

			int prevLen= strlen(bigBuff);
			sprintf(bigBuff+prevLen, buff, 
					num,
					rand()%100,
					rand()%5,
					rand()%1000,
					rand()%1000,
					rand()%1000,
					rand()%1000,
					rand()%1000,
					rand()%1000,
					rand()%1000
				   );
			int currentLen = strlen(bigBuff);
			dateLenArray[i] = currentLen;

			printf("prevLen: %d \t currentLen: %d\n", prevLen, currentLen);
			printf("%s\n", bigBuff);
		//	send(sfp, bigBuff, strlen(bigBuff),0);
			if(i%m==0 && i!=0){
				send(nfp, bigBuff, strlen(bigBuff),0);
				sleep(T);
			}
		}
	}

	bzero(&c_add,sizeof(struct sockaddr_in));

}
close(nfp);  
close(sfp);
return 0; 
}



