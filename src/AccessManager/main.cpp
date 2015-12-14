////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2007
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
#if 0
#include "AccessManager/AccessMgr.h"
#include "Alarm/Alarm.h"
#include "AppMgr/App.h"
#include "DataStore/StoreMgr.h"
#include "Debug/Debug.h" 
#include "Porting/GetTime.h"
#include "SingletonClass/SingletonMgr.h"    
#include "SQLite/Ptrs.h"
#include "SQLite/DataStoreSQLite.h"
#include "Util/OmnNew.h"
#include "XmlParser/XmlItem.h"
  
#include <sys/types.h>
#include <regex.h>
 
void printHelp()
{
	cout << "Usage: AccessMgr -a <localAddr> -p <localPort> -f <dbfile> [-n <numPorts>] [-m <maxConns>]" << endl;
}

int printSegment( const OmnString & str ) 
{
    OmnString sub;
    int i;  

    std::cout << "string=\"" << str << "\". " ;
    i=1;    
    while(str.getSegment(i,',',sub))
    {
	        std::cout << "seg" << i << "=\"" << sub << "\". " ;
	        i++;    
	}

	std::cout << std::endl ;
    return 0;
}

void strtest()
{

    printSegment("a");
    printSegment("");
    printSegment(",");
    printSegment("a,");
    printSegment(",b");
    printSegment("a,b");
    printSegment("a,,");
    printSegment("a,b,");
    printSegment(",c,e");
    printSegment(",x,");
    printSegment(",,t");
	exit(0);
}


void testreg()
{
	cout << "Enter the regular expression: " << endl;
	char reg[101];
	cin >> reg;

	cout << "Enter the string: " << endl;
	char str[101];
	cin >> str;

	regex_t regex;
	if (regcomp(&regex, reg, REG_EXTENDED|REG_NOSUB))
	{
		cout << "Regular expression incorrect" << endl;
		exit(0);
	}

	if (regexec(&regex, str, 0, 0, 0) == 0)
	{
		cout << "success" << endl;
	}
	else
	{
		cout << "Failed" << endl;
	}

	exit(0);
}

#include <unistd.h>

int 
main(int argc, char **argv)
{
	if(strcmp(argv[argc-1], "-s") != 0)
	{
		if(daemon(1, 0))
		{
			OmnAlarm << "Failed to start the daemon application" << enderr;
			return -1;	
		}
	}

	OmnApp theApp(argc, argv);
	try
	{
		theApp.startSingleton(OmnNew OmnStoreMgrSingleton());
	}

	catch (const OmnExcept &e)
	{
		OmnAlarm << "Failed to start the application: " 
			<< e.toString() << enderr;
		return 0;
	}
	
	// testreg();
	//strtest();
	// 
	// Parse Command Line
	//
	// -a <local address>
	// -p <local port>
	// -n <number of ports>
	//
	OmnIpAddr localAddr; 
	char * dbfile=0 ;
	int localPort = -1;
	int numPorts = 1;
	int maxConns = 1010;
	int index = 0;
	while (index < argc)
	{
		if (strcmp(argv[index], "-a") == 0)
		{
			localAddr = OmnIpAddr(argv[index+1]);
			index += 2;
			continue;
		}
		
		if (strcmp(argv[index], "-p") == 0)
		{
			localPort = atoi(argv[index+1]);
			index += 2;
			continue;
		}
		
		if (strcmp(argv[index], "-n") == 0)
		{
		 	numPorts = atoi(argv[index+1]);
			index += 2;
			continue;
		}

		if (strcmp(argv[index], "-m") == 0)
		{
		 	maxConns = atoi(argv[index+1]);
			index += 2;
			continue;
		}
	
		if (strcmp(argv[index], "-f") == 0)
		{
		 	dbfile = argv[index+1];
			index += 2;
			continue;
		}

		index++;
	}
	
	if ( !dbfile )
	{
		cout << "No db file specified!" << endl;
		printHelp();
		return -1;
	}
	
	if (!localAddr.isValid())
	{
		cout << "No local address specified!" << endl;
		printHelp();
		return -1;
	}
	
	if (localPort <= 0)
	{
		cout << "Invalid local port or local port is not specified: " 
			<< localPort << endl;
		printHelp();
		return -1;
	}

	if (numPorts < 0 || numPorts > 5)
	{
		numPorts = 5;
	}

	if (maxConns <= 0)
	{
		maxConns = 1010;
	}

	//OmnDataStoreMySQL::startMySQL("test", "test", "test");
	OmnDataStoreSQLite::startSQLite(dbfile);

	OmnString errmsg;
	AosAccessMgr accMgr(dbfile, localAddr, localPort, numPorts, maxConns);
	if (!accMgr.connect(errmsg))
	{
		OmnAlarm << "Failed to start Access Manager: " << errmsg << enderr;
		return -1;
	}
	if (!accMgr.startReading())
	{
		OmnAlarm << "Failed to start Access Manager: " << errmsg << enderr;
		return -1;
	}

	// 
	// This is the application part
	//
	theApp.appLoop();
	theApp.exitApp();
	return 0;
} 

#endif 
