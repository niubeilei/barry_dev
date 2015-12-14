////////////////////////////////////////////////////////////////////////////
////
//// Copyright (C) 2005
//// Packet Engineering, Inc. All rights reserved.
////
//// Redistribution and use in source and binary forms, with or without
//// modification is not permitted unless authorized in writing by a duly
//// appointed officer of Packet Engineering, Inc. or its derivatives
////
//// File Name: ApplicationProxyTester.cpp
//// Description:
////   
////
//// Modification History:
//// 11/16/2006      Created by Chen Ding
////
//////////////////////////////////////////////////////////////////////////////
#include "TimeMgr/Tester/TimeMgrTorturer.h"

#include "Tester/Test.h"
#include "Tester/TestMgr.h"
#include "Util/Random.h"
#include "Util/RandomSelector.h"
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

AosTimeMgrTorturer::AosTimeMgrTorturer()
{
}

bool AosTimeMgrTorturer::start()
{
    OmnBeginTest << "Begin the TimeMgr Torturer Testing";
    mTcNameRoot = "TimeMgr_Torturer";

	test();
	return true;
}


bool 
AosTimeMgrTorturer::test()
{
	int tries = 20;

	while (tries >0)
	{
		int ret = 0;
		int GmtOffset = OmnRandom::nextInt(-2,2);
		time_t Current;
//		struct tm *ptm;
		int GMT;
		int SendTime;
		int delta_upper_limit;
		int update_on;
		int singletime;
		time_t  NewTime;
		delta_upper_limit = 5;
		update_on = 3;
	//	if ((fp = fopen("Time.log","w+")) == NULL)
	//	{
	//		printf("Can't open Time.log!\n");
	//		exit(0);
	//	}

		Current = time(NULL);
//	Current += 60;
//		ptm = localtime(&Current);
		GMT = Current + GmtOffset;
		SendTime = Current - 3;
//	printf("*****%d\n", settime(ptm));
		singletime = ((Current - SendTime) >> 1);
//	aos_alarm(eAosMD_DenyPage, eAosAlarm_DenyPageLengthExceed,
//			"Contents too long: %d", strlen(deny_page_contents));
		if (Current - SendTime > delta_upper_limit)
		{
			printf("Too long time for net transmission delay!\n");
		//	return 0;
		//    ret = 0;
		}
		else if (fabs(GMT + singletime - Current) <= update_on)
		{
			printf("No need to change!\n");
		//	return 0;
		//	ret = 0;
		}
		else
		{
			NewTime = GMT + singletime;
			stime(&NewTime);
			printf("The NewTime is: %d\n", NewTime);
//		fprintf(fp,"%d,%d",GMT,NewTime);
	//		printf("the time is: %d:%d:%d\n", ptm->tm_hour,ptm->tm_min,ptm->tm_sec);
		//	return 1;
			ret = 1;
		}
	
		OmnTC(OmnExpected<int>(0),OmnActual<int>(ret)) << "Message:\n" 
			<< tries << " " <<endtc;
		
		tries--;
	}

	return true;
}

