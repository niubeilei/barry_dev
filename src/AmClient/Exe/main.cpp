////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2007
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
// 03/31/2007 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////

#include "AmClient/AmCltApi.h"

#include "Alarm/Alarm.h"
#include "AmClient/AmClient.h"
#include "AmClient/AmServer.h"
#include "aosUtil/Tracer.h"
#include "AppMgr/App.h"
#include "Debug/Debug.h"
#include "Porting/ThreadDef.h"
#include "Porting/Sleep.h"
#include "SingletonClass/SingletonMgr.h"
#include "Thread/ThreadShellMgr.h"
#include "Util/OmnNew.h"
#include "Util/IpAddr.h"

#include <stdio.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <stdlib.h>
#include <sqlite3.h>
#include <stdlib.h>

void printusage()
{
	cout << "AmClient -addr <addr> -port <port> -np <numPorts> -name <name>" << endl;
}


int 
main(int argc, char **argv)
{	

	OmnApp theApp(argc, argv);
	try
	{
		theApp.startSingleton(OmnNew OmnThreadShellMgrSingleton());
	}

	catch (const OmnExcept &e)
	{
		cout << "****** Failed to start the application: " << e.toString() << endl;
		OmnAlarm << "Failed to start the application: " << e.toString() << enderr;
		theApp.exitApp();
		return 0;
	}

	int idx = 0;
	OmnString type;
	OmnString configFilename;
	const int maxServers = 10;
	OmnString addr[maxServers];
	const char * 	  zAddr[maxServers];
	int		  port[maxServers];
	int		  numPorts[maxServers];
	OmnString name[maxServers];
	const char * 	  zName[maxServers];
	int       numAddrs = 0;
	int       nnumPorts = 0;
	int       numNumPorts = 0;
	int       numNames = 0;

	while (idx < argc)
	{
		if (strcmp(argv[idx], "-help") == 0)
		{
			printusage();
			theApp.exitApp();
			return 0;
		}

	//	if (strcmp(argv[idx], "-torturer") == 0)
	//	{
	//		type = "Torturer";
	//		idx++;
	//		continue;
	//	}

		if (strcmp(argv[idx], "-addr") == 0)
		{
			addr[numAddrs++] = argv[idx+1];
			idx += 2;
			continue;
		}

		if (strcmp(argv[idx], "-port") == 0)
		{
			port[nnumPorts++] = (u32)atoi(argv[idx+1]);
			idx += 2;
			continue;
		}

		if (strcmp(argv[idx], "-np") == 0)
		{
			numPorts[numNumPorts++] = atoi(argv[idx+1]);
			idx += 2;
			continue;
		}

		if (strcmp(argv[idx], "-name") == 0)
		{
			name[numNames++] = argv[idx+1];
			idx += 2;
			continue;
		}

		idx++;
	}

	if (numAddrs == 0)
	{
		cout << "No servers specified" << endl;
		printusage();
		theApp.exitApp();
		exit(0);
	}

	if (numAddrs != nnumPorts ||
		numAddrs != numNumPorts ||
		numAddrs != numNames)
	{
		cout << "Server information incorrect" << endl;
		theApp.exitApp();
		exit(0);
	}

//	AosAmClient am;
	char conn_str[256];
	for (int i=0; i<numAddrs; i++)
	{
//		am.addServer(OmnIpAddr(addr[i]), port[i], numPorts[i], name[i]);
		snprintf(conn_str, 256, "%s:%d:%d:%s\0", addr[i].data(), port[i], numPorts[i], name[i].data());
		zName[i] = name[i].data();
		zAddr[i] = addr[i].data();
	}

	int retCode = aos_am_clt_init(zAddr, port, numPorts, zName, numAddrs);
	if(0 != retCode)
	{
		printf("ReturnCode[%d]\n", retCode);
		printf("ReturnCode[%d] Input:%s ", retCode, conn_str);
		return 0;
	}
//		am.start();
	while (1)
	{
		AosAmRespCode::E code;
		OmnString errmsg;
		char 		zErrMsg[256];

		cout << "Enter User: " << endl;
		char user[101];
		cin >> user;

		cout << "Enter Password: " << endl;
		char password[101];
		cin >> password;

//		if (am.authUser(user, password, code, errmsg))
		if (0 == aos_am_clt_user_authenticate(user, password, zErrMsg, 256))
		{
			cout << "Allowed" << endl;
		}
		else
		{
			cout << "Not allowed: " 
				<< code << ". " << errmsg << endl;
		}

		cout << "Enter App: " << endl;
		char app[101];
		cin >> app;

		cout << "Enter Operation: " << endl;
		char opr[101];
		cin >> opr;

		cout << "Enter Resource: " << endl;
		char rsc[101];
		cin >> rsc;

//		if (!am.isGood())
		if (aos_am_clt_test_conn())
		{
			cout << "The client connects to no servers" << endl;
			theApp.exitApp();
			exit(0);
		}

//		if (am.userBasedAccess(app, user, opr, rsc, code, errmsg))
		if (0 == aos_am_clt_user_based_access(app, user, opr, rsc, zErrMsg, 256))
		{
			cout << "Allowed" << endl;
		}
		else
		{
			cout << "Not allowed: " 
				<< code << ". " << errmsg << endl;
		}
	}
//		am.stop();
	aos_am_clt_uninit();
	theApp.exitApp();
	return 0;
}

