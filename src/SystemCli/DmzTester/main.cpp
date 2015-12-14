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
// 11/17/2006	Created by Chen Ding
//
////////////////////////////////////////////////////////////////////////////
#include "Tester/TestMgr.h"
          
#include "AppMgr/App.h"
#include "DataStore/StoreMgr.h"
#include "Debug/Debug.h" 
#include "KernelAPI/KernelAPI.h"
#include "KernelSimu/KernelSimu.h"
#include "SingletonClass/SingletonMgr.h"    
#include "Tester/TestSuite.h"
#include "Tester/TestPkg.h"
#include "Util/OmnNew.h"
#include "Util1/TimeDriver.h"
#include "XmlParser/XmlItem.h"
  
#include "TorturerDmzSuite.h"
   
int sgCount=1;

	
int 
main(int argc, char **argv)
{
	int index=1;
	while(index<argc)
	{	
		if(strcmp(argv[index],"-h")==0)
		{
			cout<<"==================================== HELPS ===================================================================="<<endl;			
			cout<<"Usage         :  ./DmzTorturer.exe [-t [MaxRunCount]] [-h]"<<endl;
			cout<<"get usage help:  ./DmzTorturer.exe -h "<<endl;
			cout<<"-----------------------------paramater explations -----------------------------------------------------------"<<endl;
			cout<<"            -t:   Control the total run count of this torturter, by default, it value is 1"<<endl;
			cout<<"            -h:   show this help contents  "<<endl;
			cout<<"---------------------------------- examples ---------------------------------------------------------------"<<endl;
			cout<<"./DmzTorturer.exe                       (this application will run only 1 time) "<<endl;
			cout<<"./DmzTorturer.exe  -t 100               (this application will tun 100 times)"<<endl;
			cout<<""<<endl;
			cout<<"==============================================================================================================="<<endl;
			exit(0);
		}
		if(strcmp(argv[index],"-t")==0)
		{
			sgCount=atoi(argv[++index]);
			index++;
			continue;
		}
		
	}
	
	OmnApp theApp(argc, argv);
	try
	{
		 theApp.startSingleton(OmnNew OmnTimeDriverSingleton());
		 theApp.startSingleton(OmnNew OmnStoreMgrSingleton());
	}

	catch (const OmnExcept &e)
	{
		OmnAlarm << "Failed to start the application: " 
			<< e.toString() << enderr;
		return 0;
	}

	// OmnKernelApi::init();

	// 
	// This is the application part
	//
	OmnTestMgrPtr testMgr = OmnNew OmnTestMgr("SimpleTorturerTesting", "GB", "Gong Bin");
	testMgr->addSuite(AosDmzSuite::getSuite());

	cout << "Start Testing ..." << endl;
//	testMgr->setSeed(argc,argv); 
	testMgr->start();

	cout << "\nFinished. " << testMgr->getStat() << endl;

	testMgr = 0;
 
	theApp.appLoop();
	theApp.exitApp();
	return 0;
} 

 
