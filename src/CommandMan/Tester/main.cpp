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
// 12/06/2006   Created by Harry Long
//
////////////////////////////////////////////////////////////////////////////
#include "Tester/TestMgr.h"
          
#include "AppMgr/App.h"
#include "DataStore/StoreMgr.h"
#include "CommandMan/CommandMan.h" 
#include "KernelAPI/KernelAPI.h"
#include "KernelSimu/KernelSimu.h"
#include "SingletonClass/SingletonMgr.h"    
#include "Tester/TestSuite.h"
#include "Tester/TestPkg.h"
#include "Util/OmnNew.h"
#include "Util1/TimeDriver.h"
#include "XmlParser/XmlItem.h"
  
#include "CommandMan/Tester/Tester.h"
   

int 
main(int argc, char **argv)
{
	OmnString  FilePath;
	int index=1;
	while(index < argc)
	{
   	 if (strcmp(argv[index], "-f") == 0)
	  {
	    FilePath = OmnString(argv[index+1]);
		index +=2;
		continue;
	  }
  	 if (strcmp(argv[index], "-h") == 0)
	  {
	     cout << endl;
	     cout << "For Example:" << endl;
         cout <<"./CommandManTorturer.exe -f /home/command.txt   "<< endl;
	     cout << endl;
	     cout <<"-f:   the commond.txt file(include path) " << endl;
	     cout <<"-h:   for usage help" << endl;
	     cout << endl;
	     exit(0);
	     return 0;
	  }
	  index++;
	}
	
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

    // 
    // This is the application part
    //
    OmnTestMgrPtr testMgr = OmnNew OmnTestMgr("CommandManTorturerTesting", "Try", "Harry Long");
    testMgr->addSuite(CommandManSuite::getSuite(FilePath));

    cout << "Start Testing ..." << endl;
    testMgr->start();

    cout << "\nFinished. " << testMgr->getStat() << endl;

    testMgr = 0;
 
    theApp.appLoop();
    theApp.exitApp();
    return 0;
} 
