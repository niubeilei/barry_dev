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
#include "Debug/Debug.h" 
#include "Porting/GetTime.h"
#include "Porting/Process.h"
#include "XmlUtil/XmlTag.h"
#include "Porting/Sleep.h"
#include "XmlUtil/SeXmlParser.h"
#include "SEUtil/Testers/TestSuite.h"
#include "SingletonClass/SingletonMgr.h"    
#include "Tester/TestSuite.h"
#include "Tester/TestPkg.h"
#include "Util/File.h"
#include "Util/OmnNew.h"
#include "Util1/Time.h"
#include "XmlParser/XmlItem.h"
  

AosXmlTagPtr gAosAppConfig;
int gAosShowNetTraffic = 1;
int redirectOutPut();

int 
main(int argc, char **argv)
{
	redirectOutPut();
	OmnString cmd = "rm -f /usr/AOS/Data/*";
	//system(cmd);
	OmnScreen << "Cleaning~~~~~~" <<endl;
	OmnApp theApp(argc, argv);
	try
	{
	}

	catch (const OmnExcept &e)
	{
		OmnAlarm << "Failed to start the application: " << e.toString() << enderr;
		return 0;
	}

	int index = 0;
	while (index < argc)
	{
		// if (strcmp(argv[index], "-n") == 0 && index < argc-1)
		// {
		// 	tries = atoi(argv[index+1]);
		// 	index += 2;
		// 	continue;
		// }

		index++;
	}

	int max_alarms = -1;
	OmnAlarmMgr::setMaxAlarms(max_alarms);
	if (!gAosAppConfig)
	{
		AosXmlParser parser;
		OmnString str = "<config />";
		gAosAppConfig = parser.parse(str, "" AosMemoryCheckerArgs);
		aos_assert_r(gAosAppConfig, -1);
	}

	// This is the application part
	OmnTestMgrPtr testMgr = OmnNew OmnTestMgr("SEUtil/Tester", "Try", "Chen Ding");
	// testMgr->setTries(tries);
	testMgr->addSuite(AosSEUtilTestSuite::getSuite());



	cout << "Start Testing ..." << endl;
 
	testMgr->start();

	cout << "\nFinished. " << testMgr->getStat() << endl;

	testMgr = 0;
 
	OmnSleep(100000);

	theApp.exitApp();
	return 0;
} 

int redirectOutPut()                                                        
{
	close(STDOUT_FILENO);   
	OmnString filename = "output_";

	FILE* ff = popen("hostname", "r");
	char buff[128];
	int len = fread(buff, 1, 128, ff); 
	buff[len-1]=0;
	filename << buff << ".txt";
	pclose(ff);
	int fd = open(filename.data(), O_RDWR|O_TRUNC|O_CREAT, S_IREAD|S_IWRITE|S_IROTH);
	int fd2 = dup2(fd, STDOUT_FILENO);
	if (fd != fd2)
	{
		close(fd);
	}

	return 0;
}
