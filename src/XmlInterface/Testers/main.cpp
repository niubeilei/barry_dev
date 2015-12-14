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
// 	03/18/2009 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "Tester/TestMgr.h"
          
#include "AppMgr/App.h"
#include "DataStore/StoreMgr.h"
#include "Debug/Debug.h" 
#include "MySQL/Ptrs.h"
#include "MySQL/DataStoreMySQL.h"
#include "NMS/HouseKp.h"
#include "NMS/Ptrs.h"
#include "Porting/GetTime.h"
#include "SingletonClass/SingletonMgr.h"    
#include "Tester/TestSuite.h"
#include "Tester/TestPkg.h"
#include "Util/Tester/UtilTestSuite.h"
#include "Util/OmnNew.h"
#include "XmlParser/XmlItem.h"
  
#include "XmlInterface/Testers/XmlInterfaceTestSuite.h"
 
int 
main(int argc, char **argv)
{
	OmnApp theApp(argc, argv);
	try
	{
		OmnDataStoreMySQL::startMySQL("root", "chen0318", "torturer");
		theApp.startSingleton(OmnNew OmnStoreMgrSingleton());
	}

	catch (const OmnExcept &e)
	{
		OmnAlarm << "Failed to start the application: " << e.toString() << enderr;
		return 0;
	}


	// 
	// This is the application part
	//
	OmnTestMgrPtr testMgr = OmnNew OmnTestMgr("XmlInterface/Tester", "Try", "Chen Ding");
	testMgr->addSuite(AosXmlInterfaceTestSuite::getSuite());

	cout << "Start Testing ..." << endl;
 
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
	char *a1   = OmnNew char[256];
	char ** a2 = OmnNew char *[256];

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
