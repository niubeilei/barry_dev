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


void skbTest();
void bridgeTest();
void cliInterface();
int  example1();
int procKtcpvsCmd(const char *cmd, OmnString &rslt);
extern "C" int tcp_vs_proc_cmd(int argc, char **argv); 

int 
main(int argc, char **argv)
{	
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

//	OmnKernelApi::init();

	char c = 0;

	while (c != '0')
	{
		cout << "Enter your command: " << endl;
		cin >> c;

		switch (c)
		{
		case '1':
			 TestTcpApiUsr("192.168.1.30", 16000);
			 break;
		}
	}

	return 0;
} 


