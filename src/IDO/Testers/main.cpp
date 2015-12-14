////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// Description:
//   
//
// Modification History:
// 2015/03/28 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "AppMgr/App.h"

#include "Alarm/Alarm.h"
#include "Alarm/AlarmMgr.h"
#include "Debug/Debug.h" 
#include "IDOServer/IDOServer.h"
#include "JimoAPI/Jimo.h"
#include "JimoAPI/JimoIDO.h"
#include "Porting/Sleep.h"
#include "Rundata/Rundata.h"
#include "SEInterfaces/DllMgrObj.h"
#include "SEInterfaces/ClusterMgrObj.h"
#include "SingletonClass/SingletonMgr.h"    
#include "Timer/Timer.h"
#include "Util/OmnNew.h"
#include "Util1/Wait.h"
#include "Util1/TimeDriver.h"
#include "XmlParser/XmlItem.h"
#include "aosDLL/DllMgr.h"
#include "UtilData/ClusterNames.h"
#include <signal.h>  

 
int gAosShowNetTraffic = 0;

static void AosExitApp()
{
}

static bool needStop = false;
static int sgStatusFreq = 600;

void aosSignalHandler(int value)
{
	if (value == SIGALRM) needStop = true;
	if (value == 14) needStop = true;
}

using namespace Jimo;

int 
main(int argc, char **argv)
{
	signal(SIGPIPE, SIG_IGN);
	signal(SIGALRM, aosSignalHandler);

	OmnApp::appStart(argc, argv);
	OmnApp::setVersion("0.1");

	OmnApp theApp(argc, argv);

	OmnAlarmMgr::config();

	AosXmlTagPtr app_config;

	int index = 1;
	int tries = 0;
	while (index < argc)
	{
		if (strcmp(argv[index], "-config") == 0)
		{
			// '-config fname'
			app_config = OmnApp::readConfig(argv[index+1]);
			aos_assert_r(app_config, false);
			OmnApp::setConfig(app_config);
			index += 2;
			continue;
		}

		if (strcmp(argv[index], "-n") == 0 && index < argc-1)
		{
			tries = atoi(argv[index+1]);
			index += 2;
			continue;
		}
		
		if (strcmp(argv[index], "-wn") == 0 && index < argc-1)
		{
			int max_alarms = atoi(argv[index+1]);
			index += 2;
			OmnAlarmMgr::setMaxAlarms(max_alarms);
			continue;
		}
		
		index++;
	}

	OmnWait::getSelf()->start();
	OmnTimeDriver::getSelf()->start();

	AosDllMgrObj::setDllMgr(OmnNew AosDllMgr());
	AosRundataPtr rdata = OmnNew AosRundata();

	AosJimoPtr jimo = AosCreateJimoByClassname(rdata, "AosIDOServer", 1);
	aos_assert_r(jimo, -1);

	OmnScreen << "To create an IDO" << endl;
	jimo = AosCreateJimoByClassname(rdata, "AosIDOExample", 1);
	aos_assert_r(jimo, -1);
	AosIDOPtr ido = dynamic_cast<AosIDO *>(jimo.getPtr());
	aos_assert_r(ido, false);

	bool rslt = jimoCreateIDO(rdata.getPtr(), "my_ido_example", ido.getPtr(), false);
	aos_assert_r(rslt, false);

	while (1)
	{
		OmnSleep(1);
		if (needStop)
		{
			AosExitApp();
			break;
		}
	}

	theApp.exitApp();
	return 0;
} 

