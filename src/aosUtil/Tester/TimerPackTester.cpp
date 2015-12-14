////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: TimerPackTester.cpp
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include "aosUtil/Tester/TimerPackTester.h"

#include "Debug/Debug.h"
#include "Tester/Test.h"
#include "Tester/TestMgr.h"
#include "Util/OmnNew.h"
#include "Util/Random.h"
#include "Porting/Sleep.h"

#include "aosUtil/Alarm.h"
#include "aosUtil/Memory.h"
#include "aosUtil/TimerPack.h"

struct PackTimerData
{
    int expired;
    u32 expired_tick;
    u32 expected_tick;
    aos_list_head datalist;
    AosPackTimer_t * timer;
};

AosTimerPack_t  *   gTestPack;
int gmsec=1;

//static aos_lock 。。。；

static void AosTimerPackTestCallback2(struct aos_list_head * data_list)
{
	AosPackTimer_t * ptr;
	aos_list_for_each_entry(ptr, data_list, entry)
	{
		cout << "time out entry:" << &ptr->entry << endl;
		cout << "time out expires:" << ptr->expires << endl;
	}
	return;
}


static void AosTimerPackTestCallback(struct aos_list_head * data_list)
{
	AosPackTimer_t * ptr;
	int abouttick;
	int ii=0;
//lock;
	aos_list_for_each_entry(ptr, data_list, entry)
	{
		ii++;
		struct PackTimerData * data = (struct PackTimerData *)ptr->data; 
		data->expired = 1;
		data->expired_tick = gTestPack->pack_tick;
		abouttick = gTestPack->pack_tick - data->expected_tick; 
		if((data->expired_tick - data->expected_tick)>3)
		{
			cout << "timeout:" << data->expired << "expected:" << data->expected_tick << ",expired:" << data->expired_tick << endl;
			cout << "Fail  ====== data pointer ======:" << data << "======= tick About:" << abouttick << ",ii=" << ii << endl;
		}
		cout << "time out ====== data pointer ======:" << data << "======= tick About:" << abouttick << ",ii=" << ii << endl;
		//cout << "expired:" << data->expired << endl;
		//cout << "expired tick:" << data->expired_tick << endl;
		//cout << "expected tick:" << data->expected_tick << endl;
	}
//unlock;
	return;
}


bool AosTimerPackTester::start()
{
	// 
	// Test default constructor
	//
	//basicTest();
	tortureTest();
	return true;
}


bool AosTimerPackTester::basicTest()
{
	OmnBeginTest << "Test AosSlab";
	mTcNameRoot = "TimerPackTest";

	// Test the constructor
	//AosTimerPack_t * pack = AosTimerPack_create(10, AosTimerPackTestCallback);
	gTestPack = AosTimerPack_create(10, AosTimerPackTestCallback);
	OmnTC(OmnExpected<bool>(true), OmnActual<bool>(gTestPack)) << endtc;
	OmnTC(OmnExpected<bool>(true), OmnActual<bool>(gTestPack->func)) << endtc;
	OmnTC(OmnExpected<int>(10), OmnActual<int>(gTestPack->freq_msec)) << endtc;

	//
	// Start a Pack
	//
	int ret1 = AosTimerPack_start(gTestPack);
	OmnTC(OmnExpected<int>(0), OmnActual<int>(ret1)) << endtc;
	//cout << "jiffies: " << jiffies << endl;
	
	// 
	// Add a timer
	//
	
	struct PackTimerData * data;
	        if((data = (struct PackTimerData *)aos_malloc(sizeof(struct PackTimerData)))==NULL)
			{
				aos_alarm(eAosMD_Platform, eAosAlarm_MemErr,"Failed to allocate memory \n");
				return false;
			}
			        
			memset(data, 0, sizeof(struct PackTimerData));
			data->expired = 0;
			data->expired_tick = 666;
			data->expected_tick = 888;
			AOS_INIT_LIST_HEAD(&data->datalist);

	cout << "add in ====== data pointer ======:" << data << endl;
	AosPackTimer_t * ret2 = AosTimerPack_add(gTestPack, 20, (u32)data);
	data->expected_tick++;	
	AosPackTimer_t * ret22 = AosTimerPack_add(gTestPack, 22, (u32)data);
	data->expected_tick++;	
	AosPackTimer_t * ret24 = AosTimerPack_add(gTestPack, 24, (u32)data);
	data->expected_tick++;	
	AosPackTimer_t * ret4 = AosTimerPack_add(gTestPack, 40, (u32)data);
	data->expected_tick++;	
	AosPackTimer_t * ret6 = AosTimerPack_add(gTestPack, 60, (u32)data);
	data->expected_tick++;	
	AosPackTimer_t * ret8 = AosTimerPack_add(gTestPack, 80, (u32)data);
	
	//AosPackTimer_t * ret11 = AosTimerPack_add(pack, 118, (u32)&data8);
	//AosPackTimer_t * ret118 = AosTimerPack_add(pack, 11118, (u32)&data8);
	
	//OmnTC(OmnExpected<int>(0), OmnActual<int>(ret2)) << endtc;
	
	//
	// Run a timer list on the Pack slot
	// 
	//int ret3 = AosTimerPack_run((u32)pack); 
	//OmnTC(OmnExpected<int>(0), OmnActual<int>(ret3)) << endtc;

	//OmnSleep(1);
	//cout << "jiffies: " << jiffies << endl;
	return true;
}


bool AosTimerPackTester::tortureTest()
{

	OmnBeginTest << "Test AosSlab";
	mTcNameRoot = "TimerPackTest";

	if (mNumTries == 0)
	{
		mNumTries = 9999+1;
	}

	// 
	// Create a new Pack. Its frequency is randomly generated.
	//
	mfreq_msec = 999+1;	
	gTestPack = AosTimerPack_create(mfreq_msec, AosTimerPackTestCallback);	
	AosTimerPack_start(gTestPack);
	//struct PackTimerData * testData;

	AOS_INIT_LIST_HEAD(&mTimerList);

	cout << "Loop times = " << mNumTries << endl;
	while (mNumTries--)
	{
		cout << "********************* Loop Left: " << mNumTries << " ****";
		doOperations();
		checkResults();
	}

	return true; 
}


bool AosTimerPackTester::doOperations()
{
	// 
	// There are following operations:
	// 	1. Add Timer
	// 	2. Delete Timer
	// 	3. Modify Timer
	// 	4. Create a new Pack
	//
	int optSelector = OmnRandom::nextInt(1, 99+1);
	
	cout << "*********opt:" << optSelector << "**********" <<endl;
	if (optSelector <= eAddOperation)
	{
		addTimers();
	}

	else if (optSelector <= eDeleteOperation)
	{
		deleteTimers();
	}
	else if (optSelector <= eModifyOperation)
	{
		modifyTimers();
	}
	else
	{
		createNewPack();
	}

	return true;
}


bool AosTimerPackTester::addTimers()
{
	// 
	// We will add random number of timers. 
	//
	int numTimers = OmnRandom::nextInt(1, eNoTimersEachTime);

	for (int i=0; i<=numTimers; i++)
	{
		int expires_tick = OmnRandom::nextInt(1, 255); 
		int tvSelector = OmnRandom::nextInt(1, 10);
		if (tvSelector > eTv1Selector)
		{
			expires_tick *= 256;
		}

		// convert tick to msec
		u32 msec = expires_tick * mfreq_msec * ((999+1) / HZ);
		struct PackTimerData * data;
		if((data = (struct PackTimerData *)aos_malloc(sizeof(struct PackTimerData)))==NULL)
		{
			aos_alarm(eAosMD_Platform, eAosAlarm_MemErr,"Failed to allocate memory \n");
		    return false;
	    }

		memset(data, 0, sizeof(struct PackTimerData));
		data->expired = 0;
		data->expired_tick = 0;
		data->expected_tick = expires_tick + gTestPack->pack_tick;

		AOS_INIT_LIST_HEAD(&data->datalist);

		cout << "add in ++++++ data pointer ======:" << data << "++++++msec:" << msec << endl;
		AosPackTimer_t * ret2 = AosTimerPack_add(gTestPack, msec, (u32)data);
		cout << "test" << endl;
		aos_list_add_tail((aos_list_head *)&data->datalist, &mTimerList);

		gmsec++;
		//cout << "Added a timer: " << data << endl;
		//cout << "expired tick:" << data->expired_tick << endl;
		//cout << "expected tick:" << data->expected_tick << endl;
	
	//OmnTC(OmnExpected<int>(0), OmnActual<int>(ret2)) << endtc;
	
	//
	// Run a timer list on the Pack slot
	// 
	//int ret3 = AosTimerPack_run((u32)pack); 
	//OmnTC(OmnExpected<int>(0), OmnActual<int>(ret3)) << endtc;
//		data->timer = AosTimerPack_add(gTestPack, msec, (u32)&data);
		
		/*
	 	struct PackTimerData * ptr;
		aos_list_for_each_entry(ptr, &mTimerList, datalist)
	    {  
	    	cout << "add===========" << ptr  << endl;
		}
		//cout << "add pack timer: " << data->expired_tick << endl;
		*/	
	}

	return true;
}


bool AosTimerPackTester::deleteTimers()
{
	cout << "enter delete" << endl;
//	AosTimerPack_del(gTestPack, timer);
	return true;
}


bool AosTimerPackTester::modifyTimers()
{
	cout << "enter modify" << endl;
	return true;
}


bool AosTimerPackTester::createNewPack()
{
	cout << "enter create" << endl;
	/*
	AosTimerPack_free(gTestPack);
	mfreq_msec = 10;
	gTestPack = AosTimerPack_create(mfreq_msec, AosTimerPackTestCallback);
	AosTimerPack_start(gTestPack);
    AOS_INIT_LIST_HEAD(&mTimerList);
	*/

	return true;
}


bool AosTimerPackTester::checkResults()
{
//	int i = 2;
//	while(i>0)
//	{

	 	struct PackTimerData * ptr;
	 	struct PackTimerData * tmp;
	    aos_list_for_each_entry_safe(ptr, tmp, &mTimerList, datalist)
		{ 
			//cout << "check:" << ptr << endl;
			//cout << "check expired:" << ptr->expired << endl;
			if(ptr->expired == 1)
			{
				cout << "del======================================" << endl;
				int range = ptr->expired_tick - ptr->expected_tick;
				OmnTC(OmnExpected<bool>(true), OmnActual<bool>((range == 3)||(range == 1)||(range ==2))) << endtc;	
				if(!(range==3 || range==1 || range==2))
				{
					cout << "expired=1 Failed: " << ptr << ": " << ptr->expected_tick << ", " << gTestPack->pack_tick << endl;
//					exit(0);
				}
				aos_list_del(&ptr->datalist);
				aos_free(ptr);
			}
			else
			{
				OmnTC(OmnExpected<bool>(true), OmnActual<bool>(ptr->expected_tick >= ptr->expired_tick)) << endtc;
				//if (ptr->expected_tick < gTestPack->pack_tick)
				if ((ptr->expected_tick < ptr->expired_tick) && (ptr->expired !=1))
				{
					cout << "expired=0 Failed: " << ptr << ": " << ptr->expected_tick << ", " << ptr->expired_tick << endl;
//					exit(0);
				}
			}
	    }
//		OmnSleep(1);	//	sleep(mfreq_msec);
//		i--;
//	}
	return true;
}
