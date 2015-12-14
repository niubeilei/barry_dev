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
// Modification History:
// 04/17/2008: Created by Chen Ding
////////////////////////////////////////////////////////////////////////////

#include "Alarm/Alarm.h"
#include "AppMgr/App.h"
#include "Debug/Debug.h"
#include "Util/OmnNew.h"
#include "Util/File.h"
#include "util_c/global_data.h"
#include "VpdParser/AllGics.h"
  
#include <stdlib.h>


int 
main(int argc, char **argv)
{
	aos_global_data_init();

	OmnApp theApp(argc, argv);
	try
	{
	}

	catch (const OmnExcept &e)
	{
		OmnAlarm << "Failed to start the application: " << e.toString() << enderr;
		theApp.exitApp();
		return 0;
	}

	extern AosGicInputProc sgSelf;

	// cout << "sgSelf: " << sgSelf.getGicId() << endl;

	sleep(1);
	theApp.exitApp();
	return 0;
} 
 
