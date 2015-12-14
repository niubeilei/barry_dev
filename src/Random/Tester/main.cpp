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
// 11/27/2006	Created by Chen Ding
//
////////////////////////////////////////////////////////////////////////////
#include "Tester/TestMgr.h"
          
#include "AppMgr/App.h"
#include "BitmapMgr/Bitmap.h"
#include "DataStore/StoreMgr.h"
#include "Debug/Debug.h" 
#include "SEInterfaces/BitmapObj.h"    
#include "SingletonClass/SingletonMgr.h"    
#include "Tester/TestSuite.h"
#include "Tester/TestPkg.h"
#include "Util/OmnNew.h"
#include "Util1/TimeDriver.h"
#include "XmlParser/XmlItem.h"
  
#include "Random/Tester/TorturerRandomSuite.h"
   


int 
main(int argc, char **argv)
{
	OmnApp theApp(argc, argv);
	try
	{
		// theApp.startSingleton(OmnNew OmnTimeDriverSingleton());
		// theApp.startSingleton(OmnNew OmnStoreMgrSingleton());
	}

	catch (const OmnExcept &e)
	{
		OmnAlarm << "Failed to start the application: " 
			<< e.toString() << enderr;
		return 0;
	}

	AosBitmapObj::setObject(new AosBitmap());
	// This is the application part
	OmnTestMgrPtr testMgr = OmnNew OmnTestMgr("RandomTorturerTesting", "Try", "Chen Ding");
	testMgr->addSuite(AosTorturerRandomSuite::getSuite());

	cout << "Start Testing ..." << endl;
 
	testMgr->start();

	cout << "\nFinished. " << testMgr->getStat() << endl;

	testMgr = 0;
 
	theApp.exitApp();
	return 0;
} 

 
