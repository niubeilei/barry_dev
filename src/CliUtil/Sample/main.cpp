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

#include "CliUtil/CliUtil.h"
#include "CliUtil/CliUtilProc.h"

#include "Alarm/Alarm.h"
#include "AppMgr/App.h"
#include "CliUtil/CliWrapper.h"
#include "Debug/Debug.h"
#include "Porting/ThreadDef.h"
#include "Porting/Sleep.h"
#include "SingletonClass/SingletonMgr.h"
#include "Util/OmnNew.h"
#include "Util/IpAddr.h"

static char *appname[16] = {"test"};

int JustTest(char *data, unsigned int *optlen, struct aosUserLandApiParms *parms, 
					char *errmsg, const int errlen)
{
	unsigned int index = 0;
	char* buf = CliUtil_getBuff(data);
	// char local[256];
	//strcpy(local, parms->mStrings[0]);
	//CliUtil_checkAndCopy(buf, &index, *optlen, local, strlen(local));
	
	CliUtil_sprintf(buf, &index, *optlen, "just a test: %d", 1);
	CliUtil_sprintf(buf, &index, *optlen, "%s", "test");

	*optlen = index;
	return 0;
}


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
	CliUtil_initCli((char**)appname, 1, 0);
	CliUtil_regCliCmd("just test", JustTest);
	
	// 
	// Chen Ding, 05/23/2007
	//
	for (int i=0; i<20; i++)
	{
		OmnString rslt;
		OmnString cmd = "?";
		if (aos_run_cli(cmd, rslt))
		{
			cout << "Command: \"" << cmd << "\" executed correctly" << endl;
		}
		else
		{
			cout << "Command: \"" << cmd << "\" failed: " << rslt << endl;
		}
	}

	theApp.appLoop();
	theApp.exitApp();
	return 0;
} 

