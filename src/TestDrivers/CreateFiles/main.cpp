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

#include "Tester/TestMgr.h"
          
#include "AosTester/AosTestSuite.h"
#include "AppMgr/App.h"
#include "DataStore/StoreMgr.h"
#include "Debug/Debug.h" 
#include "NMS/HouseKp.h"
#include "NMS/Ptrs.h"
#include "Porting/Sleep.h"
#include "Porting/GetTime.h"
#include "SingletonClass/SingletonMgr.h"    
#include "Tester/TestSuite.h"
#include "Tester/TestPkg.h"
#include "Util/OmnNew.h"
#include "Util1/Wait.h"
#include "UtilComm/TcpClient.h"
#include "UtilComm/TcpServer.h"
#include "UtilComm/TcpTrafficGen.h"
#include "UtilComm/TcpTrafficGenListener.h"
#include "UtilComm/TrafficGenThread.h"
#include "XmlParser/XmlItem.h"
#include "Util/File.h"
  

static OmnString sgStr(1000000, 'c', true);

int 
main(int argc, char **argv)
{
	OmnString fn;
	u64 size = 0;

	OmnApp theApp(argc, argv);
	try
	{
	}

	catch (const OmnExcept &e)
	{
		OmnAlarm << "Failed to start the application: " 
			<< e.toString() << enderr;
		return 0;
	}

	int index = 0;
	while (index < argc)
	{
		if (strcmp(argv[index], "-name") == 0)
		{
			fn = argv[index+1];
			index += 2;
			continue;
		}

		if (strcmp(argv[index], "-size") == 0)
		{
			size = atoll(argv[index+1]);
			index += 2;
			continue;
		}

		index++;
	}

	if (fn == "")
	{
		cout << "Please specify file name" << endl;
		theApp.exitApp();
		return -1;
	}

	if (size == 0)
	{
		cout << "Please specify size" << endl;
		theApp.exitApp();
		return -1;
	}

	u64 idx = 0;
	OmnFile f(fn, OmnFile::eCreate);
	if (!f.isGood())
	{
		cout << "Failed to open the file: " << fn << endl;
		theApp.exitApp();
		return -1;
	}

	while (idx < size)
	{
		f.append(sgStr);
		idx += 1000000;

		if (idx % 100000000 == 0)
		{
			cout << "Size(M): " << idx/1000000 << endl;
		}
	}

	theApp.exitApp();
	return 0;
} 
 
