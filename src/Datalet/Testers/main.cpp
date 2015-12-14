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
// 2013/03/28 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "Tester/TestMgr.h"
          
#include "Alarm/AlarmMgr.h"
#include "AppMgr/App.h"
#include "Datalet/Testers/TestSuite.h"
#include "DataStore/StoreMgr.h"
#include "Debug/Debug.h" 
#include "NMS/HouseKp.h"
#include "NMS/Ptrs.h"
#include "Porting/GetTime.h"
#include "Rundata/Rundata.h"
#include "Random/RandomUtil.h"
#include "SingletonClass/SingletonMgr.h"    
#include "Tester/TestSuite.h"
#include "Tester/TestPkg.h"
#include "Timer/Timer.h"
#include "Util/OmnNew.h"
#include "Util/File.h"
#include "Util/ExeReport.h"
#include "Util/MemoryChecker.h"
#include "Util/CompareFun.h"
#include "Util/BuffArray.h"
#include "XmlParser/XmlItem.h"
#include <vector>
#include <algorithm>
#include <functional>
#include <iostream>
using namespace std;
  
struct Entry123
{
	vector<u32> entries;
	void append(const u32 i) {entries.push_back(i);}
	void print()
	{
		for (u32 i=0; i<entries.size(); i++)
		{
			cout << entries[i] << ":";
		}
		cout << endl;
	}
};

void test()
{
	typedef hash_map<const u64, Entry123, u32_hash, u32_cmp> map_t;
	typedef hash_map<const u64, Entry123, u32_hash, u32_cmp>::iterator itr_t;

	map_t map;

	Entry123 ee;
	ee.append(100);
	ee.append(200);
	map[10] = ee;

	itr_t itr = map.find(10);
	itr->second.append(300);

	itr = map.find(10);
	itr->second.print();
}


int 
main(int argc, char **argv)
{
	AosMemoryChecker::getSelf();
	cout << "Current thread id: " << OmnGetCurrentThreadId() << endl;

	OmnApp theApp(argc, argv);

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

	// This is the application part
	OmnTestMgrPtr testMgr = OmnNew OmnTestMgr("Value/Tester", "Try", "Chen Ding");
	testMgr->setTries(tries);
	testMgr->addSuite(AosDataletTestSuite::getSuite("all"));

	cout << "Start Testing ..." << endl;
 
	testMgr->start();

	cout << "\nFinished. " << testMgr->getStat().data() << endl;

	testMgr = 0;
 
	AosExeReport report(10);
	theApp.appLoop();
	theApp.exitApp();
	return 0;
} 

