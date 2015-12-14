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
#include "CliDeamon/Tester3/CliDaemonTestSuite.h"  
#include "CliDeamon/Tester3/INIFile.h" 
#include "CliDeamon/Tester3/Constants.h"  
#include "Thread/Mutex.h"
#include "Util/OmnNew.h"

//global definition

UserListEntry gUserListEntry;

//end
int 
main(int argc, char **argv)
{
    OmnIpAddr 	clientAddr("192.168.2.151");
  	OmnIpAddr 	serverAddr("192.168.2.165");
  	OmnString	filePath;
  	OmnString     logFilePath("log_");
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
  
	//read the user list

	char *p, *q;
	int numOfUserList = 0;
	
	//initialize the user link list
	AOS_INIT_LIST_HEAD(&(gUserListEntry.list));
	
	AosINIFile *pFile = new  AosINIFile(filePath.data(), OPEN_RD);
	if (!pFile->OpenConfig())
    {
		cout<<"open config file error."<<endl;
		return -1;
	}
	cout<<"starting to parse"<<endl;
	while ((p = pFile->ReadConfig(SECTION_USER)) != NULL)
    {
		//parse the user and password
		cout<<"config file:"<<p<<endl;
		q = strstr(p,":");
		*q = '\0';
		q++;
		//user
		UserListEntry *pEntry = new UserListEntry;
		strcpy(pEntry->userName, p);
		//password
		strcpy(pEntry->userPassword, q);
		//session id
		strcpy(pEntry->sessionId, DEFAULT_SESSION_ID);
		//right
		strcpy(pEntry->right, RIGHT_USER);
		//timout
		pEntry->timeout = DEFAULT_TIMEOUT;
		//add user info to list
		aos_list_add(&pEntry->list, &(gUserListEntry.list));
		numOfUserList++;
	}
    	

	//init the lock
    OmnMutexPtr mutex;
	mutex = OmnNew OmnMutex;
  	
	//application starting
  	OmnTestMgrPtr testMgr = OmnNew OmnTestMgr("UtilTester", "Try", "wu xia");
  	UserListEntry *pos;
	struct aos_list_head *tmp = NULL;
	int i = 0;
	tmp = &(gUserListEntry.list);
	
	aos_list_for_each_entry(pos, tmp,list)
    {
		//set the current user
		testMgr->addSuite(AosCliDaemonTestSuite::getSuite(serverAddr,
  					serverPort,
					clientAddr,
					clientPort + i,
					maxNumOfLogin,
					filePath,
					logFilePath<<i,
					numOfTries,
					pos,
					mutex));
		i++;
    }
	

  	cout << "Start Testing ..." << endl;
 
  	testMgr->start();

  	cout << "\nFinished. " << testMgr->getStat() << endl;

  	testMgr = 0;
 
  	theApp.appLoop();
  	theApp.exitApp();
	
  	return 0;
} 
