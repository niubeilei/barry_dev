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
// 11/22/2006 Created by Tony Ji
//
////////////////////////////////////////////////////////////////////////////


#include "Alarm/Alarm.h"
#include "AppMgr/App.h"
#include "aosUtil/Tracer.h"
#include "Debug/Debug.h"
#include "Porting/Sleep.h"
#include "SingletonClass/SingletonMgr.h"
#include "Util/OmnNew.h"
#include "Util/IpAddr.h"
#include "UtilComm/ConnBuff.h"
#include "UtilComm/TcpClient.h"

#include <stdio.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <stdlib.h>
#include <pwd.h>
#include <termios.h>

void cliInterface(const OmnTcpClientPtr &dstServer,const OmnString &sessionId,const OmnString username);
bool connectToServer(const OmnTcpClientPtr &cliConn);
bool login(OmnTcpClientPtr &cliConn,const OmnString &username, const OmnString &password,OmnString & sessionId);

bool sendCmd(const OmnString &cmd, const OmnTcpClientPtr &dstServer,const OmnString &sessionId, const OmnString &username);
void showCmd(const OmnString &cmd,OmnConnBuffPtr revBuff,const OmnString &sessionId ,const OmnTcpClientPtr &dstServer);


#define CLIDEAMON_NEED_AUTH

#ifdef CLIDEAMON_NEED_AUTH
enum
{
	eSignature1 = 0xfb,
	eSignature2 = 0xfe,
	eSessionLength = 8
};
#else
enum
{
	eSignature1 = '\n',
	eSignature2 = '\n',
	eSessionLength = 0
};
#endif


int 
main(int argc, char **argv)
{
	OmnString username;
	OmnString password;
	OmnIpAddr serverIp("127.0.0.1");

	int index = 1;
	while (index < argc)
	{
		if (strcmp(argv[index], "-u") == 0 && 
			index+1 < argc)
		{
			username = OmnString(argv[index+1]);
			index += 2;
			continue;
		}
		
		if (strcmp(argv[index], "-p") == 0&& 
			index+1 < argc)
		{
			password = OmnString(argv[index+1]);
			index += 2;
			continue;
		}
		
		if (strcmp(argv[index], "-server") == 0&& 
			index+1 < argc)
		{
			serverIp = OmnIpAddr(argv[index+1]);
			if(serverIp == OmnIpAddr::eInvalidIpAddr)
			{
				cout << "Server can not be :" << argv[index+1] << endl;
				return 0;
			}
			index += 2;			
			continue;
		}

		if (strcmp(argv[index], "-h") == 0 || strcmp(argv[index], "--help") == 0)
		{
            cout << "Usage: ls [OPTION]... [FILE]..." << endl;
            cout << "   OPTIONS:" << endl;
            cout << "       -u [username]       set username, default is the current linux user" << endl;
            cout << "       -p [password]       set password." << endl;
            cout << "       -s [serverIp]       set server ip address, default is localhost." << endl;
            cout << "       -h(--help)          show help message." << endl;
            return 0;
        }
		
		index++;
	}
	if(username == "")
	{
		// get username from system.
		struct passwd *pwd_st=getpwuid(getuid());
		username = pwd_st->pw_name;
	}
	
	if(password == "")
	{
	    password = getpass("password:");
	}
	
	OmnApp theApp(argc, argv);
	try
	{
	}

	catch (const OmnExcept &e)
	{
		cout << "****** Failed to start the application: " << e.toString() << endl;
		OmnAlarm << "Failed to start the application: " << e.toString() << enderr;
		return 0;
	}
	
	OmnTcpClientPtr			cliConn;
	OmnString 				sessionId;
	
	OmnString 				name = "server_cli_daemon";
	int 					remotePort = 28000;

//	cout << remoteIp.toString() << endl ;

	cliConn = OmnNew OmnTcpClient(	name, 
									serverIp, 
									remotePort ,
									1, 
									OmnTcp::eNoLengthIndicator);
	if(!connectToServer(cliConn))
	{
		cout << "can not connect to server." << endl;
		exit(-1);
	}
	#ifdef CLIDEAMON_NEED_AUTH
	if(!login(cliConn,username,password,sessionId))
	{
		cout << "can not login." << endl;
		exit(-1);
	}
	#endif
	password = "";	
	cliInterface(cliConn,sessionId,username);
	theApp.exitApp();
	return 0;
} 
	
bool connectToServer(const OmnTcpClientPtr &cliConn)
{
	// 
	// 1. Establish the connection
	//
	
	OmnString err;
	bool connected = false;
	int tries = 5;
	while(tries)
	{
		if(cliConn->connect(err))
		{
//			cout << "Connected!" << endl;
			connected = true;
			break;
		}
		tries--;
		sleep(1);
	}
	if(!connected)
	{
		return false;
	}
	return true;
}


void 
cliInterface(const OmnTcpClientPtr &dstServer,const OmnString &sessionId,const OmnString username)
{
	OmnString line;

	while (1)
	{
        line = readline("[ipacketengine]$");

        if (line != "")
        {
            add_history(line);
        }
//			OmnString username = pwd_st->pw_name;
//		    cmd << " " << username;
//			cout << "The input is: " << cmd << endl;
		sendCmd(line,dstServer,sessionId,username);
		// 
		// Send to the daemon
		//
		// Wait for response
		//
	}
}

bool login(OmnTcpClientPtr &cliConn,const OmnString &username, const OmnString &password,OmnString & sessionId)
{	//
	// login
	//
	int sec = 10 ;//eAosSinglePmi_CliTimeOut;
	bool timeout = false;
	bool connBroken = false;
	OmnString sendBuff;
	sendBuff << "00000000user login:" 
		<< username << ":" 
		<< password 
		<< (char)eSignature1 
		<< (char)eSignature2;
	int sendLen = sendBuff.length();
	cliConn->writeTo(sendBuff, sendLen);
	OmnConnBuffPtr revBuff = new OmnConnBuff();
	if (!cliConn->readFrom(revBuff, sec,
						timeout, connBroken, false)||timeout)
	{
		OmnAlarm << "cannot successfully get login resp from cli server" << enderr;
		return false;
	}
	if(revBuff && (! (*revBuff->getBuffer() == '\r' ||*revBuff->getBuffer() == '\n')))	
	{
		// get login response.
		if(revBuff->getDataLength() <= eSessionLength + 2)
		{
			OmnAlarm << "login resp error:" << revBuff->getDataLength() << enderr;
			return false;
		}		
		
		sessionId = OmnString(revBuff->getBuffer(),eSessionLength);
		OmnString response(revBuff->getBuffer()+eSessionLength,
						   revBuff->getDataLength()-eSessionLength -2);
		if(response == "user not exist or passwd error")
		{
			OmnAlarm << "login resp error" << enderr;
			return false;
		}		
	}
	
	OmnString changeLevelCmd = sessionId;
	changeLevelCmd << "user level set config"
		<< (char)eSignature1 
		<< (char)eSignature2;
	sendLen = changeLevelCmd.length();
	cliConn->writeTo(changeLevelCmd, sendLen);
	revBuff = new OmnConnBuff();
	if (!cliConn->readFrom(revBuff, sec,
						timeout, connBroken, false)||timeout)
	{
		OmnAlarm << "cannot successfully get change level resp from cli server" << enderr;
		return false;
	}
	if(revBuff && (! (*revBuff->getBuffer() == '\r' ||*revBuff->getBuffer() == '\n')))	
	{
		// get login response.
		if(revBuff->getDataLength() < eSessionLength + 2)
		{
			OmnAlarm << "change level error:"  << revBuff->getDataLength() << enderr;
			return false;
		}		
		
		OmnString response(revBuff->getBuffer()+eSessionLength,
						   revBuff->getDataLength()-eSessionLength -2);

		if(response == "can not set as configer, another one occupies")
		{
			OmnAlarm << "another config level is online."  << enderr;
			return false;
		}		
		
	}
	return true;
}

bool 
sendCmd(const OmnString &cmd, const OmnTcpClientPtr &dstServer,const OmnString &sessionId, const OmnString &username)
{
	OmnString errmsg;
	OmnRslt rslt;
	int sec = 10 ;//eAosSinglePmi_CliTimeOut;
	bool timeout = false;
	bool connBroken;
	
	if (!dstServer->isConnGood())
	{
		dstServer->closeConn();
		dstServer->connect(errmsg);
		if (!dstServer->isConnGood())
		{
			//alarm and return;
			OmnAlarm << "Failed to process connection" << rslt.getErrmsg() << enderr;
			return false;
		}
	}
	OmnString sendBuff = sessionId;

	//if it's a command for secured shell
	//client should send the username as a part of command
	if(cmd.hasPrefix("secured shell"))
	{
		sendBuff << cmd 
				<<" " 
				<<	username
				<< (char)eSignature1 
				<< (char)eSignature2;
	}
	else
	{
		//if user wants to logout
		//send a logout message to cli daemon
		if(cmd.hasPrefix("logout") || cmd.hasPrefix("exit") || cmd.hasPrefix("quit") || cmd.hasPrefix("user logout"))
		{
			sendBuff <<	"user logout"
					<<	(char)eSignature1
					<<	(char)eSignature2;
		}
		else
		{
			//send cmd normally
			sendBuff << cmd
					<< (char)eSignature1 
					<< (char)eSignature2;
		}
	}
	
	int sendLen = sendBuff.length();

	dstServer->writeTo(sendBuff, sendLen);

	OmnConnBuffPtr revBuff = new OmnConnBuff();
	if (!dstServer->readFrom(revBuff, sec,
						timeout, connBroken, false)||timeout)
	{
		//alarm and return
		showCmd(cmd,revBuff,sessionId,dstServer);
		OmnAlarm << "cannot login to cli server" << rslt.getErrmsg() << enderr;
		return false;
	}
	if(revBuff)	
	{
		showCmd(cmd,revBuff,sessionId,dstServer);
	}
	
	return true;
}

void
showCmd(const OmnString &cmd,OmnConnBuffPtr revBuff,const OmnString &sessionId ,const OmnTcpClientPtr &dstServer)
{
	if(revBuff)
	{
		if(revBuff->getDataLength() == eSessionLength + 2)
		{
			cout << "logout!" << endl;
			exit(0);
		}
		if(revBuff->getDataLength() < eSessionLength + 2)
		{
			cout << "response length not right:";
			cout << OmnString(revBuff->getBuffer(),revBuff->getDataLength()) << endl;
		}
		else
		{
			cout << OmnString(revBuff->getBuffer() + eSessionLength,revBuff->getDataLength() - eSessionLength - 2) << endl;
		}		
	}
	else
	{
		if(!connectToServer(dstServer))
		{
			cout << "Connection lost!" << endl;
			exit(-1);
		}
	}
}
