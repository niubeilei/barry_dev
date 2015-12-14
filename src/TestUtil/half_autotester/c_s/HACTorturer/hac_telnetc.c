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
#define MAXLINE 4096
#define TRY 1000

//enum COMMAND {LIST = 1,UNKNOW,QUIT,GET,PUT,CWD,D,PRINT,U};

struct telnet_client {
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
	struct telnet_client c;
	int n;
	//enum COMMAND command; 
	char type[512];
	char homenode[1024];
	char tmp[1024];
	char logfile[1024];
	char scrfile[1024];
	int trynum;
	int interval = 3;
	FILE *script;

	int user_len;
	int pass_len;
	char user[BUFFER];
	char pass[BUFFER];
	char *puser;
	char *ppass;
	char *ruser;
	char *rpass;
	char *SEP = ":";
	char line[MAXLINE];
	char cmd[MAXLINE];
	char *ENDFLAG = "\n";
	char *DIRCMD = "cd ";
	char *MKDIRCMD = "mkdir ";
	char *RMDIRCMD = "rmdir ";
	char *QUITCMD = "exit";
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
		printf("usage : hac_telnetc <type> <ip> <port> \'<user>\' \'<passwd>\' <alias> <homenode> <trynum> [<interval>]\n\tDescription: press-test the telnet_audit of HAC\n\t     <type>: dir|grep\n\t <homenode>: if type=dir, means the target directory, if type=grep, means the grep directory\n\t <interval>: optional.The interval(second) between the continuous commands. The default value is 3.\n\t    Example: hac_telnetc dir 172.22.250.216 2021 \'ttt:ftp' 'tttttt:ftp' test incoming/123 1000 3\n");
		exit(1);
	}

	strcpy(type,argv[1]);
	strcpy(user,argv[4]);
	puser = strtok(user,SEP);
	ruser = strtok(NULL,SEP);
	strcpy(pass,argv[5]);
	ppass = strtok(pass,SEP);
	rpass = strtok(NULL,SEP);
	strcpy(homenode,argv[7]);
	trynum = atoi(argv[8]);
	if(argc == 10)
		interval = atoi(argv[9]);

	strcpy(logfile,argv[6]);
	strcat(logfile,".telnet.log");
	console = fopen(logfile,"w+"); 

	if(strcasecmp(type,"dir")==0)
	{

		// Part One: the login step
		strcpy(scrfile,argv[6]);
		strcat(scrfile,".telnet.script");
		script = fopen(scrfile,"w+"); 

		fputs("#!/usr/bin/env expect\n",script);
		if(interval != 0)
			sprintf(line, "set timeout %d\n", interval);
		fputs(line, script);
		fputs("set a [lindex $argv 0]\n",script);
		fputs("set p [lindex $argv 1]\n",script);
		fputs("spawn telnet $a $p\n",script);
		fputs("#HAC Proxy login\n",script);
		fputs("expect \"*login:*\"\n",script);
		sprintf(line, "send \"%s\\n\"\n", puser);
		fputs(line, script);
		fputs("expect \"*assword:*\"\n",script);
		sprintf(line, "send \"%s\\n\"\n", ppass);
		fputs(line, script);
		fputs("#Real Server login\n",script);
		fputs("expect \"*login:*\"\n",script);
		sprintf(line, "send \"%s\\n\"\n", ruser);
		fputs(line, script);
		fputs("expect \"*assword:*\"\n",script);
		sprintf(line, "send \"%s\\n\"\n", rpass);
		fputs(line, script);
		fputs("send \"ls\\n\"\n",script);
		fputs("expect\n",script);

		// Part Two: the cmd operation step

		strcpy(cmd, DIRCMD);
		strcat(cmd, homenode);
		sprintf(line, "send \"%s\\n\"\n", cmd);
		fputs(line, script);
		fprintf(console,"%s", line); 
		fputs("expect\n",script);

		int i = 0;
		while(i<trynum)
		{ 

			int j;

			for(j=0; j<5; j++)
			{
				strcpy(cmd, MKDIRCMD);
				strcat(cmd, DIRNAME[j]);

				sprintf(line, "send \"%s\\n\"\n", cmd);
				fputs(line, script);
				fprintf(console,"%s", line); 

				fputs("expect *@*\n",script);
			}

			for(j=0; j<5; j++)
			{
				strcpy(cmd, RMDIRCMD);
				strcat(cmd, DIRNAME[j]);

				sprintf(line, "send \"%s\\n\"\n", cmd);
				fputs(line, script);

				fprintf(console,"%s", line); 

				fputs("expect *@*\n",script);
			}

			i++;
		}

		fputs("send \"exit\\n\"\n",script);
		fputs("expect \"logout\"\n",script);
		fputs("exit\n",script);
		fputs("expect eof\n",script);
		fclose(script); 


		sprintf(line, "chmod 755 %s", scrfile);
		system(line);

		sprintf(line, "./%s %s %s", scrfile, argv[2], argv[3]);
		system(line);
	}
	else if(strcasecmp(type,"grep")==0)
	{
		int i = 0;
		char cmd[BUFFER];
		char tmp[BUFFER];
		//system("chmod 755 wget");
		while(i<trynum)
		{
			//strcpy(cmd,"wget --ftp-user=lwang:ftp --ftp-password=lwang:ftp --no-passive-ftp ftp://");
			//strcpy(cmd,"./wget --ftp-user=ftp --ftp-password=ftp --no-passive-ftp ftp://");
			sprintf(tmp, "grep 111111 %s -r", homenode);
			strcpy(cmd, tmp);
			fprintf(console,"%s\n", cmd); 
			system(cmd);
			if(interval != 0)
				sleep(interval);
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
