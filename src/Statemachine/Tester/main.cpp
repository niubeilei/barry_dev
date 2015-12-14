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
          
#include "Alarm/AlarmMgr.h"
#include "AppMgr/App.h"
#include "DataStore/StoreMgr.h"
#include "Debug/Debug.h" 
#include "NMS/HouseKp.h"
#include "NMS/Ptrs.h"
#include "Porting/GetTime.h"
#include "SingletonClass/SingletonMgr.h"    
#include "Tester/TestSuite.h"
#include "Tester/TestPkg.h"
#include "Util/Tester/UtilTestSuite.h"
#include "Util/OmnNew.h"
#include "XmlParser/XmlItem.h"
  
 
int test111(const bool dd)
{
	cout << dd << endl;
	return -1;
}

int test();
int testFile64();

int 
main(int argc, char **argv)
{
	char *ss;
	test111(ss);

	testFile64();
	OmnApp theApp(argc, argv);
	try
	{
 
//		OmnDataStoreMySQL::startMySQL("root", "chen0318", "repository");
		theApp.startSingleton(OmnNew OmnStoreMgrSingleton());
	}

	catch (const OmnExcept &e)
	{
		OmnAlarm << "Failed to start the application: " << e.toString() << enderr;
		return 0;
	}

	int index = 1;
	int tries = 0;
	while (index < argc)
	{
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


	// 
	// This is the application part
	//
	OmnTestMgrPtr testMgr = OmnNew OmnTestMgr("Util/Tester", "Try", "Chen Ding");
	testMgr->setTries(tries);
	testMgr->addSuite(OmnUtilTestSuite::getSuite());

	cout << "Start Testing ..." << endl;
 
// test();
	testMgr->start();

	cout << "\nFinished. " << testMgr->getStat() << endl;

	testMgr = 0;
 
	theApp.appLoop();
	theApp.exitApp();
	return 0;
} 

 
int test()
{


	// array is a pointer to char *, or array[0] is a char*;
	char *a1   = new char[256];
	char ** a2 = new char *[256];

	for (int i=0; i<256; i++)
	{
		a1[i] = 'a';
		a2[i] = a1;
	}

	for (int k=0; k<256; k++)
	{
		for (int m=0; m<256; m++)
		{
			char a = a2[k][m];
			a = 1;
		}
	}

	return 0;
}


int testFile64()
{
	// http://www.mkssoftware.com/docs
	FILE *ff = fopen64("test.txt", "w");
	u64 pos = 1000;
	pos *= 1000;
	pos *= 1000;
	pos *= 10;
	cout << "pos: " << pos << endl;
	fseeko64(ff, pos, SEEK_END);
	::fwrite("12345", 1, 5, ff);
	fclose(ff);
	return 0;
}

