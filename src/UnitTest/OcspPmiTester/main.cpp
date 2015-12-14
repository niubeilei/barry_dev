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

#include "Alarm/Alarm.h"
#include "AppMgr/App.h"
//#include "CliSimuLib/KernelApiFunc.h"
#include "Debug/Debug.h"
#include "KernelAPI/KernelAPI.h"
#include "KernelInterface/CliProc.h"
#include "KernelInterface/KernelInterface.h"
#include "KernelSimu/KernelSimu.h"
#include "Porting/ThreadDef.h"
#include "PKCS/CertMgr.h"
#include "PKCS/CertChain.h"
#include "SingletonClass/SingletonMgr.h"
#include "Tester/TestSuite.h"
#include "Tester/TestPkg.h"
#include "Tester/TestMgr.h"
//#include "UnitTest/UnitTestSuite.h"
#include "UnitTest/OcspPmiTester/OcspPmiUnitTestSuite.h"
#include "Util/OmnNew.h"
#include "Util/IpAddr.h"

#include <readline/readline.h>
#include <readline/history.h>

void cliInterface();
    
int 
main(int argc, char **argv)
{	
    pid_t pid;
	if((pid=fork())==0){
//if(1)	{
		OmnApp theApp(argc, argv);
		try
		{
			theApp.startSingleton(OmnNew OmnKernelInterfaceSingleton());
			theApp.startSingleton(OmnNew OmnCliProcSingleton());
		}
	
		catch (const OmnExcept &e)
		{
			cout << "****** Failed to start the application: " << e.toString() << endl;
			OmnAlarm << "Failed to start the application: " << e.toString() << enderr;
			return 0;
		}
	
	    OmnKernelApi::init();
	
		OmnTestMgrPtr testMgr = OmnNew OmnTestMgr("PMITester", "PMI integration testing", "Hugo");  
	    //testMgr->addSuite(OmnOcspCrlUnitTestSuite::getSuite());
	    //trafficGenIp,	serverIp,	bouncerIp,	PMIServerIp
	    testMgr->addSuite(OmnOcspPmiUnitTestSuite::getSuite(OmnIpAddr("192.168.1.240"),//traffic gen ip
	    													  OmnIpAddr("192.168.1.235"),//server ip
	    													  OmnIpAddr("192.168.1.245"),//bouncer ip
	    													  OmnIpAddr("192.168.1.245")));// pmi server ip
	
	    cout << "Start Testing ..." << endl;
	
	    testMgr->start();
	
	    cout << "\nFinished. " << testMgr->getStat()<< endl;
	
		cout << "**************************************" << endl;
	    testMgr = 0;
	
		theApp.appLoop();
	
		return 0;
	}
	else
	{
		return 0;
	}
	return 0;
} 

void
cliInterface()
{
	OmnString rslt;                                                                                                        
    bool skip = false;                                                                                                      
    char *line;                                                                                         

    while (1)                                                                                                         
    {                                                                                                      
        line = readline("[OCSPCRLTESTING_SSLProxy]$");  
                                                                                                           
        if (strcmp(line, "")!=0)                                                                           
        {                                                                                                              
            add_history(line);                                                                                              
        }                                                                                                                   
                                                                                                                  
        rslt = "";                                                                                                          
                                                                                                                        
        if (strlen(line) > 3)                                                                                                
        {                                                                                                                      
            skip = false;                                                                                                 
            OmnCliProc::getSelf()->runCli(line, rslt);                                                            
            cout << rslt << endl;                                                                                         
        }                                                                                                                    
        else                                                                                                          
        {                 
            skip = true;                                                                                                    
        }
                                                                                 
        free(line);                                                                                                     
    }    
}
 

