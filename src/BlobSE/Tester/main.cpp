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
// 2014/11/25 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>    /* For O_RDWR */
#include <unistd.h>   /* For open(), creat() */
#include <sys/stat.h>

#include "Tester/TestMgr.h"
#include "BlobSE/Tester/BlobSETester.h"
#include "alarm_c/alarm.h"
#include "Alarm/AlarmMgr.h"
#include "AppMgr/App.h"
#include "Porting/Sleep.h"
#include "BlobSE/Tester/TestSuite.h"
#include "Tester/TestPkg.h"
#include "Util1/TimeDriver.h"
#include "XmlUtil/XmlTag.h"
#include <getopt.h>
#include "BitmapMgr/Bitmap.h"
#include "SEInterfaces/BitmapObj.h"
#include "Util1/Wait.h"
#include "aosDLL/DllMgr.h"
#include "SEInterfaces/DllMgrObj.h"

int gAosShowNetTraffic = 1;

AosXmlTagPtr	gApp_config;
i64				gTestDuration = 0;
u32				gRandomSeed = 0;
bool			gTestRaftAPI = false;
bool			gUnitTest = false;
bool			gSanityCheck = true;

void usage(char* p)
{
	printf("Usage: %s\n", p);
	printf("\t-c config xml file\n");
	printf("\t-l redirect log to file, eg: -l file_name\n");
	printf("\t-r using raft api\n");
	printf("\t-s random seed, for triggering bugs\n");
	printf("\t-t test duration, -1 for infinite, 0 for batch test\n");
	printf("\t-u compaction unit test\n");
}


int redirectOutPut(OmnString name)
{
	close(STDOUT_FILENO);
	//Jozhi 2014/09/18 change all printing to [base_dir]/proc_log directory
//	OmnString base_dir = OmnApp::getAppBaseDir();
//	OmnString parent_dir = OmnApp::getParentDir(base_dir);
//	OmnString dir_name = OmnApp::getDirName(base_dir);
//	OmnString log_dir;
//	log_dir << parent_dir << "/proc_log/";
//	OmnApp::createDir(log_dir);
	OmnString filename;
	filename << name;
	setbuf(stdout, 0);

//	char buff[128];
//	struct tm * ptm;
//	time_t rawtime;
//	time(&rawtime);
//	ptm = gmtime(&rawtime);
//	sprintf(buff, "%02d%02d%02d%02d%02d", ptm->tm_mon+1,
//			ptm->tm_mday, (ptm->tm_hour+8)%24, ptm->tm_min, ptm->tm_sec);
//	filename << buff << ".txt";
	int fd = open(filename.data(), O_RDWR|O_TRUNC|O_CREAT, S_IREAD|S_IWRITE|S_IROTH);
	int fd2 = dup2(fd, STDOUT_FILENO);
	if (fd != fd2)
	{
		close(fd);
	}
	return 0;
}



int
main(int argc, char **argv)
{
	int opt = 0;

	while((opt = getopt(argc, argv, "c:s:t:l:ru")) != -1)
	{
		switch (opt)
		{
		case 'c':
			gApp_config = OmnApp::readConfig(optarg);
			if(!gApp_config)
			{
				std::cout << "Can not read config file." << endl;
				usage(argv[0]);
				return -1;
			}
			OmnApp::setConfig(gApp_config);
			break;
		case 's':
			gRandomSeed = atoi(optarg);
			break;
		case 't':
			gTestDuration = atoi(optarg);
			break;
		case 'l':
			redirectOutPut(optarg);
			break;
		case 'r':
			gTestRaftAPI = true;
			break;
		case 'u':
			gUnitTest = true;
			break;
		default:
			std::cout << "Invalid argument." << endl;
			usage(argv[0]);
			return -99;
			break;
		}
	}

	OmnApp theApp(argc, argv);
	AosBitmapObj::setObject(OmnNew AosBitmap());
	OmnAlarmMgr::config();
	AosDllMgrObj::setDllMgr(OmnNew AosDllMgr());
	OmnWait::getSelf()->start();
	OmnTimeDriver::getSelf()->start();
	OmnSleep(1);	//waiting for the time driver to start working
	// This is the application part
	OmnTestMgrPtr testMgr = OmnNew OmnTestMgr("BlobSE/Tester", "BlobSE Test", "White");
	testMgr->addSuite(AosBlobSETestSuite::getSuite());

	cout << "Start Testing ..." << endl;

	testMgr->start();

	cout << "\nFinished. " << testMgr->getStat().data() << endl;

	testMgr = 0;
	theApp.exitApp();
	return 0;
}

