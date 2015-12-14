////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: ServerTester.cpp
// Description:
// Below are the list of all the CLI commands:
//
// MemMgr
//1. mem mgr set record time <N>[HDM]
//2. mem mgr set record [on|off]
//3. mem mgr show mem <time_start> <time_end> <interval>
//4. mem mgr show process <time_start> <time_end> [<top_process_num>]
//5. mem mgr set busy threshold <time> <usage>
//6. mem mgr set normal threshold <time> <usage> 
//7. mem mgr threshold show
//


	
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include "SystemCli/MemMgrTester/MemMgrTester.h"

#include "aosUtil/Alarm.h"
#include "aosUtil/Tracer.h"
#include "aosUtil/Types.h"
#include "KernelSimu/in.h"
#include "Debug/Debug.h"
#include "KernelInterface/CliProc.h"
#include "Porting/Sleep.h"
#include "Random/RandomInteger.h"
#include "Tester/Test.h"
#include "Tester/TestPkg.h"
#include "Tester/TestMgr.h"
#include "Thread/Mutex.h"
#include "Util/OmnNew.h"
#include "Util/IpAddr.h"
#include "Util/Random.h"
#include "Util1/Wait.h"
#include "Util1/Timer.h"
#include "UtilComm/Ptrs.h"
#include "KernelSimu/timer.h"


#define BUG
// 
// CLI Command selector definition
//

const int hourOfDay = 24;
const int minOfDay = 60 * 24;
const int secOfDay = 24 * 60 * 60;
const int secOfMin = 60;
const int secOfHour = 60 * 60;

static OmnString sgCliSelectorDef = 
	    "<Elements>"
	        "<Element>1,  10</Element>"		//1. mem mgr set record time <N>[HDM]                                 
	        "<Element>2,  10</Element>"		//2. mem mgr set record [on|off]                                      
	        "<Element>3,  15</Element>"		//3. mem mgr show mem <time_start> <time_end> <interval>              
	        "<Element>4,  15</Element>"		//4. mem mgr show process <time_start> <time_end> [<top_process_num>] 
	        "<Element>5,  25</Element>"		//6. mem mgr set busy threshold <time> <usage>                        
	        "<Element>6,  25</Element>"		//7. mem mgr set normal threshold <time> <usage>                      
			"<Element>7,  0</Element>"		//8. mem mgr threshold show
		"</Elements>";

int AosMemMgrTester::mRecordTime = 0;
int AosMemMgrTester::mRecordInterval = 0;
int AosMemMgrTester::mOnTime = 0;
int AosMemMgrTester::mOffTime = 0;
bool AosMemMgrTester::mStatus = true;
int AosMemMgrTester::mBusyTime = 0;
int AosMemMgrTester::mBusyUsage = 0;
int AosMemMgrTester::mNormalTime = 0;
int AosMemMgrTester::mNormalUsage = 0;

AosMemMgrTester::AosMemMgrTester()
{
}

void
AosMemMgrTester::upgradeTime(int a)
{
	int nowTime;
	int interval;
	
	nowTime = time(NULL);
	
	if (mRecordTime == 0 || mStatus == false)
		return;
		
	interval = mRecordTime;
	
	if (mOnTime < nowTime-interval)
		mOnTime = nowTime - interval;
	
	cout << "mOnTime =" << mOnTime 
		<< "mOffTime"	<< mOffTime << endl;
	return;
}

bool 
AosMemMgrTester::start()
{
	
//	alarm(3);
//	signal(SIGALRM, upgradeTime);
  	OmnTimerObjPtr thisPtr(this, false);
	OmnTimerSelf->startTimer("cputester",3,0,thisPtr,0);
	
	basicTest();
	return true;
}

bool 
AosMemMgrTester::basicTest()
{
	OmnBeginTest << "Test MemMgr";
	// 
	// Create the CLI command selector
	//
	AosRandomInteger cliSelector("CliSelector", sgCliSelectorDef,100);

	int count = 0;
	bool rslt;
	while (count < sgCount)
	{
//		cout << "AosMemMgrTester::mOnTime:" << AosMemMgrTester::mOnTime << 
	//		" AosMemMgrTester::mOffTime:" << AosMemMgrTester::mOffTime << endl;
	cout <<"========================== Now run the :"<<count+1<<" time =================================================="<<endl;
		int index = cliSelector.nextInt();

		switch (index)
		{
		case eMemMgrSetRecord: 			
			 rslt = runMemMgrSetRecord();
			 break;
		case eMemMgrRecordStatus :			
			 rslt = runMemMgrRecordStatus();
			 break;
		case eMemMgrShowMem :			
			 rslt = runMemMgrShowMem();
			 break;
		case eMemMgrShowProcess :			
			 rslt = runMemMgrShowProces();
			 break;
		case eMemMgrBusyThreshold:
			rslt = runMemMgrBusyThreshold();
			break;
		case eMemMgrNormalThreshold:
			rslt = runMemMgrNormalThreshold();
			break;
		case eMemMgrShowThreshold:
			rslt = runMemMgrShowThreshold();
			break;
		default:
			 OmnAlarm << "Invalid index: " << index << enderr;
			 break;
		}
		count++;
		OmnWait::getSelf()->wait(0,1000);
	}

	return true;
}


bool 
AosMemMgrTester::runMemMgrSetRecord()
{
	//
	//1. mem mgr set record time <N>[HDM]
	//
	OmnString cmd("mem mgr set record time ");
	
	int interval, intTime;
	OmnString timeString;
	OmnString sInterval;
	bool isCorrect = true;
	
	genRecordInterval(sInterval, interval, isCorrect);
	genRecordTime(timeString, intTime, isCorrect);
	
	if (intTime/interval < 10 || intTime/interval > 1000000)
	{
		isCorrect = false;
	}
	
	if (isCorrect)
	{
		mRecordTime = intTime;
		mRecordInterval = interval;
	}
	
	cmd << interval << " " << timeString;
	aosRunCli(cmd, isCorrect);
	if (isCorrect)
	{
		cout << cmd << endl;
	}
	
	return true;
}


bool 
AosMemMgrTester::runMemMgrRecordStatus()
{
	//
	//2. mem mgr set record [on|off]
	//
	
	OmnString status;
	OmnString cmd("mem mgr set record ");
	bool isCorrect = true;
	
	if (RAND_INT(1, 100) < 80)
	{
		if (RAND_INT(1, 100) < 80)
		{
			status = "on";
			mStatus = true;
		}
		else
		{
			status = "off";
			mStatus = false;
		}
	
		mOnTime = time(NULL);
		mOffTime = 0;
	}
	else
	{
		status = OmnRandom::nextLetterStr(2, 5);
		if (status == "on" || status == "off")
			isCorrect = true;
		else
			isCorrect = false;
	}
	
	cmd << status;
	aosRunCli(cmd, isCorrect);

	if (isCorrect)
		cout << cmd << endl;
	return true;
}


bool 
AosMemMgrTester::runMemMgrShowMem()
{
	//
	//3. mem mgr show cpu <time_start> <time_end> <interval>
	//
	OmnString cmd, startTime, endTime, interval;
	time_t sTime, eTime;
	bool isCorrect;
	int intInterval;
	
	cmd = "mem mgr show mem ";
	isCorrect = true;
	
	if (AosMemMgrTester::mOnTime == 0)
		return true;
	
	genStartEndTime(startTime, endTime, sTime, eTime, isCorrect);

	//create interve
	genInterval(sTime, eTime, interval, intInterval, isCorrect);
	if (isCorrect)
	{
		if (intInterval == 0 
			|| (intInterval > (eTime - sTime)/120) 
			|| (intInterval < mRecordInterval)
			|| (eTime - sTime) == 0)
		{
			isCorrect = false;
		}
	}
		
	cmd << startTime << " " << endTime << " " << interval;
	aosRunCli(cmd, isCorrect);
	if (isCorrect)
		cout << cmd << endl;
	
	return true;
}


bool 
AosMemMgrTester::runMemMgrShowProces()
{
	//
	//4. mem mgr show process <time_start> <time_end> [<top_process_num>]
	//
	OmnString cmd, startTime, endTime;
	int  numProcess;
	time_t sTime, eTime;
	bool isCorrect;
	
	if (AosMemMgrTester::mOnTime == 0)
		return true;
	
	isCorrect = true;
	cmd = "mem mgr show process ";

	genStartEndTime(startTime, endTime, sTime, eTime, isCorrect);
	//create the number of process
	if (RAND_INT(0, 100) > 20)
	{
		numProcess = RAND_INT(1, 20);
	}
	else
	{
		numProcess = RAND_INT(21, 100);
		isCorrect = false;
	}

	if (eTime-sTime <= 0)
		isCorrect = false;
	
	cmd << startTime << " " << endTime << " " << numProcess;
	aosRunCli(cmd, isCorrect);
	if (isCorrect)
		cout << cmd << endl;
	return true;
}


bool 
AosMemMgrTester::runMemMgrShowThreshold()
{
	//5. mem mgr show threshold 
	
	OmnString cmd;
	cmd = "mem mgr show threshold";
	aosRunCli(cmd, true);        
}


bool 
AosMemMgrTester::runMemMgrBusyThreshold()
{
	//6. mem mgr set busy threshold <time> <usage>    
	OmnString cmd, time, usage;
	int intTime;
	int intUsage;
	cmd = "mem mgr set busy threshold ";
	
	bool isCorrect = true;
	genRecordTime(time, intTime, isCorrect);
	
	genUsage(usage, intUsage, isCorrect);

	cmd << time << " " << usage;
	aosRunCli(cmd, isCorrect);
	if (isCorrect)
	{
		mBusyTime = intTime;
		mBusyUsage = intUsage;
		cout << cmd << endl;
	}
	return true;
}


bool 
AosMemMgrTester::runMemMgrNormalThreshold()
{
	//7. mem mgr set normal threshold <time> <usage>
	OmnString cmd;
	OmnString time;
	OmnString usage;
	int intTime;
	int intUsage;
	
	cmd = "mem mgr set normal threshold ";
	bool isCorrect = true;
	
	genRecordTime(time, intTime, isCorrect);
	genUsage(usage, intUsage, isCorrect);
	
	cmd << time << " " << usage;

	aosRunCli(cmd, isCorrect);
	
	if (isCorrect)
	{
		cout << cmd << endl;
		mNormalTime = intTime;
		mNormalUsage = intUsage;
	}
	return true;
}

bool
AosMemMgrTester::OmnGetTimeStr(OmnString &timeStr, time_t &time)
{
	struct tm timeStruct;
	
	memcpy(&timeStruct, localtime(&time), sizeof(struct tm));
	timeStr = "";
	timeStr << 1900+timeStruct.tm_year;
	if (timeStruct.tm_mon < 10)
	{
		timeStr << 0 << timeStruct.tm_mon;
	}
	else
	{
		timeStr << timeStruct.tm_mon; 
	}
	
	if (timeStruct.tm_mday < 10)
	{
		timeStr << 0 << timeStruct.tm_mday << "-" 
		<< timeStruct.tm_hour << ":" << timeStruct.tm_min << ":" << timeStruct.tm_sec;
	}
	else
	{
		timeStr << timeStruct.tm_mday << "-" 
		<< timeStruct.tm_hour << ":" << timeStruct.tm_min << ":" << timeStruct.tm_sec;
	}
	
	return true;
}


bool
AosMemMgrTester::genStartEndTime(OmnString &startTime, 
								 OmnString &endTime, 
								 time_t &sTime, 
								 time_t &eTime, 
								 bool &isCorrect)
{
	// 
	// Start time is expressed as:
	// 	yyyymmdd-hh:mm:ss
	//
	int randNum;
	int nowTime;
	
	if (mOnTime == 0)
		return true;
	if (mOffTime == 0)
		nowTime = time(NULL);
	else
		nowTime = mOffTime;
	
	randNum = RAND_INT(0, 100);	
	
	if (randNum > 20)
	{
		sTime = RAND_INT(mOnTime, nowTime);
		eTime = RAND_INT(sTime, nowTime);
		if (sTime == eTime)
			isCorrect = false;
		OmnGetTimeStr(startTime, sTime);
		OmnGetTimeStr(endTime, eTime);
	}
	else if (randNum > 15)
	{
		sTime = RAND_INT(mOnTime, nowTime);
		eTime = RAND_INT(mOnTime, sTime);
		OmnGetTimeStr(startTime, sTime);
		OmnGetTimeStr(endTime, eTime);
		isCorrect = false;
	}
	else
	{
		sTime = 0;
		eTime = 0;
		startTime = OmnRandom::nextLetterStr(5, 10);
		endTime = OmnRandom::nextLetterStr(5, 10);
		isCorrect = false;
	}
	
	return true;
}


bool
AosMemMgrTester::genInterval(time_t sTime,
							 time_t eTime, 
							 OmnString &interval, 
							 int &intInterval,
							 bool &isCorrect)
{
	int randNum = RAND_INT(1, 100);
	int maxInterval;
	
	if (randNum > 20)
	{
		randNum = RAND_INT(1, 100);
		if (randNum > 70)
		{
			maxInterval = (eTime - sTime)/4;
			intInterval = RAND_INT(1, maxInterval);
		}
		else if (randNum > 30)
		{
			maxInterval = (eTime - sTime)/2;
			intInterval = RAND_INT(1, maxInterval);
		}
		else 
		{
			maxInterval = eTime - sTime;
			intInterval = RAND_INT(1, maxInterval);
		}
	}
	else if (eTime-sTime > 0)
	{	
		maxInterval = (eTime - sTime)*2;
		intInterval = RAND_INT((eTime-sTime), maxInterval);
		isCorrect = false;
	}
	else
	{
		intInterval = RAND_INT(1, 100);
		isCorrect = false;
	}
	
	interval = "";
	interval << intInterval;
	
	return true;
}bool
AosMemMgrTester::genRecordTime(OmnString &time, 
						       int &intTime,
							   bool &isCorrect)
{
	int elm;
	int randNum = OmnRandom::nextInt(1, 100);
	char unit;
	int digital;
	
	if (randNum > 20)
	{
			
		//
		//Correct
		//
	
		elm = RAND_INT(1, 4);
		switch(elm)
		{
			case 1:
				 unit = (RAND_BOOL())?'d':'D';
 				 
 				 randNum = RAND_INT(1, 100);
 				 if (randNum > 50)
 				 {
 				 	digital = RAND_INT(1, 4);
 				 }
				 else if (randNum > 10)
 				 {
 				 	digital = RAND_INT(5, 10);
 				 }
				 else 
				 {
				 	digital = RAND_INT(10, MAX_INT/86400);
 				 }
 				 
 				 intTime = digital * 86400;
				 break;
				
			case 2:
				 unit = (RAND_BOOL())?'h':'H';
 				 
 				 randNum = RAND_INT(1, 100);
 				 if (randNum > 50)
 				 {
 				 	digital = RAND_INT(1, 24);
 				 }
				 else if (randNum > 10)
 				 {
 				 	digital = RAND_INT(25, 100);
 				 }
				 else 
				 {
				 	digital = RAND_INT(101, MAX_INT/3600);
 				 }
 				 
 				 intTime = digital * 3600;
				 break;	
					
			case 3:
				 unit = (RAND_BOOL())?'m':'M';
				 
 				 randNum = RAND_INT(1, 100);
 				 if (randNum > 50)
 				 {
 				 	digital = RAND_INT(1, 50);
 				 }
				 else if (randNum > 10)
 				 {
 				 	digital = RAND_INT(51, 200);
 				 }
				 else 
				 {
				 	digital = RAND_INT(201, MAX_INT/60);
 				 }
 				 
 				 intTime = digital * 60;
 				 break;
					
			case 4:
				 unit = (RAND_BOOL())?'s':'S';

 				 randNum = RAND_INT(1, 100);
 				 if (randNum > 50)
 				 {
 				 	digital = RAND_INT(1, 500);
 				 }
				 else if (randNum > 10)
 				 {
 				 	digital = RAND_INT(501, 2000);
 				 }
				 else 
				 {
				 	digital = RAND_INT(2001, MAX_INT);
 				 }
 				 
 				 intTime = digital;
 				 break;

			case 5:
				 unit = '\0';
				 
 				 randNum = RAND_INT(1, 100);
 				 if (randNum > 50)
 				 {
 				 	digital = RAND_INT(1, 500);
 				 }
				 else if (randNum > 10)
 				 {
 				 	digital = RAND_INT(501, 2000);
 				 }
				 else 
				 {
				 	digital = RAND_INT(2001, MAX_INT);
 				 }
 				 
 				 intTime = digital;
				 break;
				 
			default:
				return true;
		}
		time << digital << unit;
	}
	else
	{
		// 
		// Incorrect time:
		// 1. incorrect unit
		// 2. not a digital string
		isCorrect = false;
		if (RAND_INT(1, 100) > 10)
		{
			unit = RAND_INT(60, 125);
			if (unit == 'd' || unit == 'D' ||
			    unit == 'h' || unit == 'H' ||
				unit == 'm' || unit == 'M' ||
				unit == 's' || unit == 'S')
			{
				unit = 'a';
			}
		
			digital = RAND_INT(1, 1000);
			time << digital << unit;
		
		}
		else
		{
			time = OmnRandom::nextLetterStr(5, 10);
		}
	}

	return true;
}


bool
AosMemMgrTester::genUsage(OmnString &usage, int &intUsage, bool &isCorrect)
{
	int randNum = OmnRandom::nextInt(1, 100);
	if (randNum > 20)
	{
		intUsage = OmnRandom::nextInt(1, 100);
		usage << intUsage;
	}
	else
	{
		intUsage = OmnRandom::nextInt(101, 200);
		usage << intUsage;
		isCorrect = false;
	}
	
	return true;
}


void        
AosMemMgrTester::timeout(const int timerId,
		                const OmnString &timerName,
		                void *parm)
{
	AosMemMgrTester::upgradeTime(3);
    OmnTimerObjPtr thisPtr(this, false);
	OmnTimerSelf->startTimer("cputester", 3, 0, thisPtr, 0);
}

bool        
AosMemMgrTester::genRecordInterval(OmnString &sInterval,
								  int &interval,
								  bool &isCorrect)
{
	int elm;
	int randNum = OmnRandom::nextInt(1, 100);
	char unit;
	int digital;
	
	if (randNum > 20)
	{
			
		//
		//Correct
		//
	
		elm = RAND_INT(1, 5);
		switch(elm)
		{
			case 1:
				 unit = (RAND_BOOL())?'d':'D';
 				 
 				 randNum = RAND_INT(1, 100);
 				 if (randNum > 70)
 				 {
 				 	digital = RAND_INT(1, 2);
 				 }
				 else if (randNum > 10)
 				 {
 				 	digital = RAND_INT(3, 10);
 				 }
				 else 
				 {
				 	digital = RAND_INT(10, MAX_INT/86400);
 				 }
 				 
 				 interval = digital * 86400;
				 break;
				
			case 2:
				 unit = (RAND_BOOL())?'h':'H';
 				 
 				 randNum = RAND_INT(1, 100);
 				 if (randNum > 70)
 				 {
 				 	digital = RAND_INT(1, 5);
 				 }
				 else if (randNum > 10)
 				 {
 				 	digital = RAND_INT(6, 10);
 				 }
				 else 
				 {
				 	digital = RAND_INT(11, MAX_INT/3600);
 				 }
 				 
 				 interval = digital * 3600;
				 break;	
					
			case 3:
				 unit = (RAND_BOOL())?'m':'M';
				 
 				 randNum = RAND_INT(1, 100);
 				 if (randNum > 70)
 				 {
 				 	digital = RAND_INT(1, 30);
 				 }
				 else if (randNum > 10)
 				 {
 				 	digital = RAND_INT(30, 120);
 				 }
				 else 
				 {
				 	digital = RAND_INT(121, MAX_INT/60);
 				 }
 				 
 				 interval = digital * 60;
 				 break;
					
			case 4:
				 unit = (RAND_BOOL())?'s':'S';

 				 randNum = RAND_INT(1, 100);
 				 if (randNum > 70)
 				 {
 				 	digital = RAND_INT(1, 100);
 				 }
				 else if (randNum > 10)
 				 {
 				 	digital = RAND_INT(101, 500);
 				 }
				 else 
				 {
				 	digital = RAND_INT(500, MAX_INT);
 				 }
 				 
 				 interval = digital;
 				 break;

			case 5:
				 unit = '\0';
				 
 				 randNum = RAND_INT(1, 100);
 				 if (randNum > 70)
 				 {
 				 	digital = RAND_INT(1, 100);
 				 }
				 else if (randNum > 10)
 				 {
 				 	digital = RAND_INT(101, 500);
 				 }
				 else 
				 {
				 	digital = RAND_INT(500, MAX_INT);
 				 }
 				 
 				 interval = digital;
				 break;
				 
			default:
				return true;
		}
		sInterval << digital << unit;
	}
	else
	{
		// 
		// Incorrect time:
		// 1. incorrect unit
		// 2. not a digital string
		isCorrect = false;
		if (RAND_INT(1, 100) > 10)
		{
			unit = RAND_INT(32, 125);
			if (unit == 'd' || unit == 'D' ||
			    unit == 'h' || unit == 'H' ||
				unit == 'm' || unit == 'M' ||
				unit == 's' || unit == 'S')
			{
				unit = 'a';
			}
		
			digital = RAND_INT(1, 1000);
			sInterval << digital << unit;
		
		}
		else
		{
			sInterval = OmnRandom::nextLetterStr(5, 10);
		}
	}
	
	return true; 
}
