///////////////////////////////////////////////////////////////
// Created by:	wu xia
// Created:		09/15/2006	
// Comments:
//			main entry of cliDeamon test
// 
// Change History:
//	09/15/2006 File Created
///////////////////////////////////////////////////////////////
#include "Tester/TestMgr.h"
          
#include "AppMgr/App.h"
#include "DataStore/StoreMgr.h"
#include "Debug/Debug.h" 
#include "KernelAPI/KernelAPI.h"
#include "KernelSimu/KernelSimu.h"
#include "Porting/GetTime.h"
#include "SingletonClass/SingletonMgr.h"    
#include "Tester/TestSuite.h"
#include "Tester/TestPkg.h"
#include "Util/OmnNew.h"
#include "Util/Random.h"
#include "Util1/TimeDriver.h"
#include "XmlParser/XmlItem.h"
#include "CliDeamon/Tester2/CliDaemonTestSuite.h"  
   
int 
main(int argc, char **argv)
{
  OmnIpAddr 	clientAddr("192.168.2.151");
  OmnIpAddr 	serverAddr("192.168.2.165");
  OmnString	filePath;
  OmnString     logFilePath("log.txt");
  int		serverPort = 28000;
  int 		clientPort = 1099;
  int		connNum;
  int		repeat;
  int		numOfTries;
  int		maxNumOfLogin = 100;
  int index = 1; 
  while (index < argc)
    {   
      if (strcmp(argv[index], "-client") == 0)
	{   
	  clientAddr = OmnIpAddr(argv[index + 1]);
	  index += 2;
	  continue;
	}
        
      if (strcmp(argv[index], "-server") == 0)
	{   
	  serverAddr = OmnIpAddr(argv[index + 1]);
	  index += 2;
	  continue;
	}

      if (strcmp(argv[index], "-serverport") == 0)
	{   
	  serverPort = atoi(argv[index + 1]);
	  index += 2;
	  continue;
	}
	if (strcmp(argv[index], "-clientport") == 0)
	{
		clientPort = atoi(argv[index + 1]);
		index += 2;
		continue;
	}
      if (strcmp(argv[index], "-conn") == 0)
	{   
	  connNum = atoi(argv[index + 1]);
	  index += 2;
	  continue;
	}

      if (strcmp(argv[index], "-repeat") == 0)
	{   
	  repeat = atoi(argv[index + 1]);
	  index += 2;
	  continue;
	}

      if (strcmp(argv[index], "-trynum") == 0)
	{   
	  numOfTries = atoi(argv[index + 1]);
	  index += 2;
	  continue;
	}
        	
      if (strcmp(argv[index], "-login") == 0)
	{
	  maxNumOfLogin = atoi(argv[index + 1]);
	  index += 2;
	  continue;
	}
      //file path
      if (strcmp(argv[index], "-file") == 0)
	{
	  filePath = OmnString(argv[index + 1]);
	  index += 2;
	  continue;
	}
      if (strcmp(argv[index], "-log") == 0)
	{
	  logFilePath = OmnString(argv[index + 1]);
	  index += 2;
	  continue;
	}
        	
      if (strcmp(argv[index], "--help") == 0)
	{   
	  cout << endl;
	  cout << "For Example:" << endl;
	  cout << endl;
	  cout << endl;
	  exit(0);
	  return 0;
	}
        
      index++;
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

  OmnKernelApi::init();
  

  //application starting
  OmnTestMgrPtr testMgr = OmnNew OmnTestMgr("UtilTester", "Try", "wu xia");
  testMgr->addSuite(AosCliDaemonTestSuite::getSuite(serverAddr,
						    serverPort,
						    clientAddr,
						    clientPort,
						    maxNumOfLogin,
						    filePath,
						    logFilePath,
						    repeat,
						    connNum,
						    numOfTries));

  cout << "Start Testing ..." << endl;
 
  testMgr->start();

  cout << "\nFinished. " << testMgr->getStat() << endl;

  testMgr = 0;
 
  theApp.appLoop();
  theApp.exitApp();
	
  return 0;
} 
