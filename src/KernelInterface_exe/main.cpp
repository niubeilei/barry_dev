////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: main.cpp
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include "Alarm/Alarm.h"
#include "AppMgr/App.h"
#include "Debug/Debug.h"
#include "KernelAPI/KernelAPI.h"
#include "KernelAPI/UserLandWrapper/TestTcpApiU.h"
#include "KernelInterface/CliProc.h"
#include "KernelInterface/KernelInterface.h"
#include "KernelSimu/KernelSimu.h"
#include "Porting/ThreadDef.h"
#include "Porting/Sleep.h"
#include "SingletonClass/SingletonMgr.h"
#include "Util/OmnNew.h"
#include "Util/IpAddr.h"

// chk 12/15/2006 Add for tcp connection
#include "UtilComm/TcpClient.h"
#include "UtilComm/ConnBuff.h"

#include <stdio.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <stdlib.h>
#include <sqlite3.h>

#include "aosUtil/Tracer.h"

#include "License/liblicense/verify.h"
#include "version.h"

void cliInterface(bool isClient);
void cliCmdInterface( bool isClient, OmnString cmd);
int procKtcpvsCmd(const char *cmd, OmnString &rslt);
extern "C" int tcp_vs_proc_cmd(int argc, char **argv); 

int find_command(int a, int b)
{
    char command[]={"find command"};
    unsigned int i;
    printf(" %s ", rl_line_buffer);
    for(i=0; i<sizeof(command); i++)
    rl_insert(1,command[i]);
    return 0;
}

void show_version(int argc, char**argv)
{
	if(argc<2)
		return;
	if( 0 == strcmp(argv[1],"-v") )
	{
		printf("%s\n",VERSION);
		exit(0);
	}
	else if( 0 == strcmp(argv[1],"-b") )
	{
		printf("%s\n",BUILDNUMBER);
		exit(0);
	}
	else if( 0 == strcmp(argv[1],"-r") )
	{
		printf("%s\n",RELEASE);
		exit(0);
	}
	return;
}
void check_license()
{

	return;
	if(verify_license())
		return;
	printf("License is incorrect.\n");
	exit(1);
	return;
}

// chk 12/15/2006 Add for command parsing
// Trim the redundant ' '|'\t'|'\n'|'\r' from the input string
int TrimFormat(OmnString &strCmd)
{
	int nPos = 0;
	OmnString strWord,strResult;

	nPos = strCmd.getWord(nPos,strWord);
	while(strWord.length() > 0)
	{
		if(strResult.length() > 0)
			strResult += " ";
		strResult += strWord;
		nPos = strCmd.getWord(nPos,strWord);
	}
	strCmd = strResult;
	return 0;
}

// Tcp to the station of Daemon
// Input:
// 	OmnString :  Input Command for inside the station
// Output:
// 	OmnString :  Output string for the reply of the TCP package
// Return:
// 	int : 		Return value for the operation
//		0	Stand for Return_OK
// 		!=0	Stand for Operation_Failure
/*int TcpDaemon(const OmnString &strCmd, OmnString &strResult, OmnString &errMsg)
{
	OmnString mutexName("cliDeamonTest");
	int DEFAULT_CLIENT_ID = 1;
	bool isTimeout = false;
	bool isConnBroken = false;
	OmnConnBuffPtr connBuffPtr;

	//open the connection
	OmnTcpClientPtr tcpClient = OmnNew OmnTcpClient(OmnIpAddr("172.22.250.222"),
						  28000,
						  1,
						  OmnIpAddr("localhost"), // 127.0.0.1 fail   172.22.250.243
						  28000,
						  1,
						  mutexName);
	tcpClient->setClientId(DEFAULT_CLIENT_ID);
	if(!tcpClient->connect(errMsg))
	{
		OmnString strTmp;
		strTmp += OmnString("Failed to connect: ");
		strTmp += OmnString("127.0.0.1:");
		strTmp += OmnString("28000. ");
		strTmp += errMsg;
		errMsg = strTmp;
		tcpClient->closeConn();
		return -1;
	}
	//starting login 
	if (!tcpClient->writeTo(strCmd.getBuffer(),strCmd.length()))
	{
		cout<<"send login info failed."<<endl;
		tcpClient->closeConn();
		return -1;
	}
	if (!tcpClient->readFrom1(connBuffPtr, 
			    isConnBroken,
			    true))
	{
		cout<<"not recieved login authorization"<<endl;
		tcpClient->closeConn();
		return -1;
	}
	tcpClient->closeConn();
	strResult = connBuffPtr->getString();
	return 0;
}
*/
int 
main(int argc, char **argv)
{	

	bool isClient = false;
	show_version(argc,argv);
	check_license();
	OmnApp theApp(argc, argv);
	try
	{
		theApp.startSingleton(OmnNew OmnKernelInterfaceSingleton());
		theApp.startSingleton(OmnNew OmnCliProcSingleton());
	}

	catch (const OmnExcept &e)
	{
		cout << "****** Failed to start the application: " << e.toString() << endl;
		OmnAlarm << "Failed to start the application: " << e.toString() << enderr;
		return 0;
	}

	int index = 1;
	OmnString cmd = "";
	int cmd_req = 0;

	while (index < argc)
	{
		if (strcmp(argv[index], "-cmd") == 0)
		{
			// chk 12/15/2006 Add for command parsing 
			// and tcp connection
			OmnString strResult;
			OmnString errMsg;
			cmd = argv[index+1];
			if(TrimFormat(cmd) != 0)
			{
				cout << "Input command is illegal! [" 
					<< cmd << "]" << endl;
				OmnAlarm << "Failed to start the application: -cmd \"" 
					<< cmd << "\"" << enderr;
				theApp.exitApp();
				return 0;
			}
            isClient = true;
			 cmd_req = 1;
			 index += 2;
			 continue;
			
			// logout directly
			//theApp.exitApp();
			//return 0;
		}
		index ++;
	}

	if (cmd_req == 0)
	{
		char c = 0;

        isClient = true;

    	rl_initialize();
    	rl_bind_key('\t', find_command);

		while (c != '0')
		{
			cliInterface(isClient);
			continue;
		}
	}
	else
	{
		cliCmdInterface(isClient,cmd);
	}

	theApp.exitApp();
	return 0;
} 

void cliCmdInterface( const bool isClient,OmnString cmd)
{
	OmnString rslt = "";

	if(isClient)
	{
		OmnCliProc::getSelf()->runCliAsClient(cmd, rslt);
	}
	else
	{
		OmnCliProc::getSelf()->runCli(cmd, rslt);
	}
	cout << rslt << endl;
}

void
cliInterface(bool isClient)
{
	OmnString rslt, cmd;
	bool skip = false;
	char *line;

	while (1)
	{
        line = readline("[ipacketengine]$");
		cmd = line;
		if(TrimFormat(cmd) != 0)
		{
			cout << "Input command is illegal! [" 
				<< cmd << "]" << endl;
			OmnAlarm << "Failed to start the application: -cmd \"" 
				<< cmd  << "\"" << enderr;
			continue;
		}

        if (strcmp(cmd, "")!=0)
        {
            add_history(cmd);
        }
		
		rslt = "";

		if (strlen(cmd) > 3)
		{
			skip = false;
			if(isClient)
			{
				OmnCliProc::getSelf()->runCliAsClient(cmd, rslt);
			}
			else
			{
				OmnCliProc::getSelf()->runCli(cmd, rslt);
			}
			cout << rslt << endl;
		}
		else
		{
			skip = true;
		}

		free(line);
	}
}

