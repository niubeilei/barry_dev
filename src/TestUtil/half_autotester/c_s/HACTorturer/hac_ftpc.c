///////////////////////////////////////////////////////////////
// Created by:	kevin
// Created:		02/01/2007	
// Comments:
//
// Change History:
//	02/01/2007 File Created
///////////////////////////////////////////////////////////////

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/file.h>
#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <malloc.h>

#include <stdlib.h>

#define BUFFER 1024
#define TRY 1000

//enum COMMAND {LIST = 1,UNKNOW,QUIT,GET,PUT,CWD,D,PRINT,U};

struct ftp_client {
	int protocol;  // sock for protocol communication
	int data;   // sock for data communication
	struct sockaddr_in address_protocol;
	struct sockaddr_in address_data;
	char buf[BUFFER];
	char path[BUFFER];
	int n;
	int len;
	char *stack[BUFFER];  //for store directory name
	int position ;
	int tell ;   // for position in list_ftp in directory process it should be set to 0
	char name[BUFFER][128]; // name length of file is 128
	char file[BUFFER];
};

FILE *console;

int main(int argc, char *argv[])
{
	int result;
	struct ftp_client c;
	int n;
	//enum COMMAND command; 
	char type[512];
	char homenode[1024];
	char tmp[1024];
	char logfile[1024];
	int trynum;
	int interval = 3;

	int user_len;
	int pass_len;
	char user[BUFFER];
	char pass[BUFFER];
	char *ENDFLAG = "\r\n";
	char *DIRCMD = "CWD ";
	char *MKDIRCMD = "MKD ";
	char *RMDIRCMD = "RMD ";
	char *QUITCMD = "QUIT";
	char *DIRNAME[] = {
		"1234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890_100_abc1",
		"1234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890_100_abc2",
		"1234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890_100_abc3",
		"1234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890_100_abc4",
		"1234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890_100_abc5"
			//"12345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890_200_abc1",
			//"12345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890_200_abc2",
			//"12345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890_200_abc3",
			//"12345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890_200_abc4",
			//"12345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890_200_abc5",
	};
	char *TESTCMD[] = {
		"CWD incoming",
		"MKD 123",
		"RMD 123",
		//	"PWD",
		//"cd",
		"QUIT"
	};

	//c.position = 0;  

	c.protocol = socket(AF_INET, SOCK_STREAM, 0);   
	c.address_protocol.sin_family = AF_INET;
	c.address_data.sin_family = AF_INET;

	if ((argc != 9) && (argc != 10))
	{
		printf("usage : hac_ftpc <type> <ip> <port> \'<user>\' \'<passwd>\' <alias> <homenode> <trynum> [<interval>]\n\tDescription: press-test the ftp_audit of HAC\n\t     <type>: dir|wget\n\t <homenode>: if type=dir, this is the directory, if type=wget, this is the target file\n\t <interval>: optional.The interval(second) between the continuous commands. The default value is 3.\n\t     Example: hac_ftpc dir 172.22.250.216 2021 \'ttt:ftp' 'tttttt:ftp' test incoming/123 1000\n");
		exit(1);
	}

	strcpy(type,argv[1]);
	strcpy(user,argv[4]);
	strcpy(pass,argv[5]);
	strcpy(homenode,argv[7]);
	trynum = atoi(argv[8]);
	if(argc == 10)
		interval = atoi(argv[9]);

	strcpy(logfile,argv[6]);
	strcat(logfile,".ftp.log");
	console = fopen(logfile,"w+"); 

	if(strcasecmp(type,"dir")==0)
	{
		c.address_protocol.sin_addr.s_addr = inet_addr(argv[2]);
		c.address_data.sin_addr.s_addr = inet_addr(argv[2]);

		c.address_protocol.sin_port = htons(atoi(argv[3]));
		c.len = sizeof(c.address_protocol);

		// Part One: the login step

		printf("connect started \n");       
		result = connect(c.protocol, (struct sockaddr *)&c.address_protocol, c.len);

		if(result == -1) {
			perror("error, check your ftp ip and port is correct\n");
			exit(1);
		}
		printf("connect succesful\n"); 
		printf("input user name: \n");  //process user name
		strcpy(tmp,"USER ");
		strcat(tmp, user);
		strcat(tmp, ENDFLAG);
		strcpy(user, tmp);
		user_len = strlen(user);
		//strcat(user, "lwang:ftp");
		//strcat(user, ENDFLAG);
		//user_len = 5+9+2;
		//user_len = getline(user+5);

		printf("input user passcode: \n");  //process user code
		strcpy(tmp,"PASS ");
		strcat(tmp, pass);
		strcat(tmp, ENDFLAG);
		strcpy(pass, tmp);
		pass_len = strlen(pass);
		//strcat(pass, "lwang:ftp");
		//strcat(pass, ENDFLAG);
		//pass_len = 5+9+2;
		//pass_len = getline(pass+5);     

		printf("user = %s,%d\n",user,user_len);
		printf("pass = %s,%d\n",pass,pass_len);

		recv(c.protocol,c.buf,BUFFER,0);
		printf("receive = %s", c.buf);

		if(c.buf[0]=='2' && c.buf[1]=='2' && c.buf[2] == '0')
		{
			//send(c.protocol,user,user_len+4,0) ;
			send(c.protocol,user,user_len,0) ;
		}

		while(1)
		{
			read(c.protocol,c.buf,BUFFER);
			printf("receive = %s", c.buf);fflush(stdout);
			if(c.buf[0]=='2' && c.buf[1]=='3' && c.buf[2] == '0')
				break;

			//send(c.protocol,pass,pass_len+4,0);
			send(c.protocol,pass,pass_len,0);
			sleep(1);
		}

		memset(c.buf,0,1);

		send(c.protocol, "type i\r\n", 8, 0); //data type ascII
		n = recv(c.protocol, c.buf, BUFFER,0);
		c.buf[n] = '\0';
		printf("set type = %s", c.buf);
		int k = 6;
		send(c.protocol, "syst\r\n", k, 0);
		n = recv(c.protocol, c.buf, BUFFER,0);
		c.buf[n] = '\0';
		printf("syst = %s",c.buf);

		// Part Two: the cmd operation step

		strcpy(c.buf, DIRCMD);
		strcat(c.buf, homenode);
		strcat(c.buf, ENDFLAG);
		fprintf(console,"ftpc_send: %s\n", c.buf); 
				printf("scmd = %s\n",c.buf);
		send(c.protocol,c.buf,strlen(c.buf),0) ;
		n = recv(c.protocol, c.buf, BUFFER,0);
		c.buf[n] = '\0';
		printf("CMD_result = %s\n",c.buf);

		int i = 0;
		while(i<trynum)
		{ 
			static int flag = 0;
			if (flag) break;

			int j;

			for(j=0; j<5; j++)
			{
				k = 6;
				send(c.protocol, "port\r\n", k, 0);
				n = recv(c.protocol, c.buf, BUFFER,0);
				c.buf[n] = '\0';
				printf("prslt = %s\n",c.buf);

				strcpy(c.buf, MKDIRCMD);
				strcat(c.buf, DIRNAME[j]);
				strcat(c.buf, ENDFLAG);
				fprintf(console,"ftpc_send: %s\n", c.buf); 
				printf("scmd = %s\n",c.buf);
				send(c.protocol,c.buf,strlen(c.buf),0);
				n = recv(c.protocol, c.buf, BUFFER,0);
				c.buf[n] = '\0';
				printf("result = %s\n",c.buf);
				if(interval != 0)
					sleep(interval);
			}

			for(j=0; j<5; j++)
			{
				k = 6;
				send(c.protocol, "port\r\n", k, 0);
				n = recv(c.protocol, c.buf, BUFFER,0);
				c.buf[n] = '\0';
				printf("prslt = %s\n",c.buf);

				strcpy(c.buf, RMDIRCMD);
				strcat(c.buf, DIRNAME[j]);
				strcat(c.buf, ENDFLAG);
				fprintf(console,"ftpc_send: %s\n", c.buf); 
				printf("scmd = %s\n",c.buf);
				send(c.protocol,c.buf,strlen(c.buf),0);
				n = recv(c.protocol, c.buf, BUFFER,0);
				c.buf[n] = '\0';
				printf("result = %s\n",c.buf);
				if(interval != 0)
					sleep(interval);
			}
			i++;
		} 

		// Part Three: the logout step
		strcpy(c.buf, QUITCMD);
		strcat(c.buf, ENDFLAG);
		fprintf(console,"ftpc_send: %s\n", c.buf); 
		send(c.protocol,c.buf,strlen(c.buf),0);
		n = recv(c.protocol, c.buf, BUFFER,0);
		c.buf[n] = '\0';
		printf("Bye_result = %s\n",c.buf);

		close(c.protocol);
		close(c.data);
	}
	else if(strcasecmp(type,"wget")==0)
	{
		int i = 0;
		char cmd[BUFFER];
		system("chmod 755 wget");
		while(i<trynum)
		{
			//strcpy(cmd,"wget --ftp-user=lwang:ftp --ftp-password=lwang:ftp --no-passive-ftp ftp://");
			//strcpy(cmd,"./wget --ftp-user=ftp --ftp-password=ftp --no-passive-ftp ftp://");
			strcpy(cmd,"./wget --ftp-user=ttt:ftp --ftp-password=tttttt:ftp --no-passive-ftp ftp://");
			strcat(cmd,argv[2]);
			strcat(cmd,":");
			strcat(cmd,argv[3]);
			strcat(cmd,"/");
			strcat(cmd,homenode);
			fprintf(console,"wget: %s\n", cmd); 
			system(cmd);
			i++;
		}

	}
	else
	{
		printf("The <type> is error!\n");
		exit(1);
	}
	fclose(console); 
	exit(0);
}
