////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
//
// Modification History:
// 10/26/2010	Create by Tank
////////////////////////////////////////////////////////////////////////////
#include "BrowserSimu/BrowserThrd.h"


#include "Util/OmnNew.h"
#include "Thread/Mutex.h"
#include "Thread/Thread.h"
#include "BrowserSimu/BrowserSimu.h"
#include "Util/String.h"


static const int sgNumAccounts = 10;
static OmnString sgUsernames[sgNumAccounts] = 
{
	"jx000049",
	"jx000031",
	"janelin",
	"zhaoqingfeng",
	"liguoyuan",
	"gufeng",
	"yanglin",
	"houhuaying",
	"gumin",
	"ssx20100823"
};

static OmnString sgPasswds[sgNumAccounts] = 
{
	"12345",
	"696758",
	"7895123",
	"12345",
	"12345",
	"12345",
	"12345",
	"12345",
	"023023",
	"4358214439" 
};

static OmnString sgHpvpds[sgNumAccounts] = 
{
	"jihong_room_jx000049",
	"jihong_room_jx000031",
	"yyy_room_janelin",
	"yyy_room_zhaoqingfeng",
	"zkyroom_liguoyuan",
	"ssx_room_gufeng",
	"ssx_room_yanglin",
	"ssx_room_houhuaying",
	"yyy_room_gumin",
	"ssx_room.302222" 
};

static OmnString sgCtnrs[sgNumAccounts] = 
{
	"yunyuyan_account", 
	"yunyuyan_account",
	"yunyuyan_account", 
	"yunyuyan_account", 
	"yunyuyan_account", 
	"sx_account", 
	"sx_account", 
	"sx_account", 
	"sx_account", 
	"sx_account" 
};

static OmnString sgObjids[sgNumAccounts] = 
{
	"zky_oidur_jx000049",
	"zky_oidur_jx000031",
	"zky_oidur_janelin",
	"zky_oidur_zhaoqingfeng",
	"zky_oidur_liguoyuan",
	"zkyoidur_gufeng",
	"zkyoidur_yanglin",
	"zkyoidur_houhuaying",
	"zky_oidur_gumin",
	"zkyuser.302222"
};
static int sgAccountIdx = 0;
static OmnMutex sgLock;

AosBrowserThrd::AosBrowserThrd(const int tid)
:
mTest(OmnNew AosBrowserSimu())
{
	mThreadId = tid;
OmnScreen << "Create thread: " << mThreadId << endl;
	OmnThreadedObjPtr thisPtr(this, false);
    mThread = OmnNew OmnThread(thisPtr, "browserThrd", 0, true, true, __FILE__, __LINE__);
    mThread->start();
}


AosBrowserThrd::~AosBrowserThrd()
{
}


bool	
AosBrowserThrd::threadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread)
{
    while (state == OmnThrdStatus::eActive)
	{
		doProcess();
	}
	return true;
}


bool	
AosBrowserThrd::signal(const int threadLogicId)
{
	return true;
}


bool    
AosBrowserThrd::checkThread(OmnString &err, const int thrdLogicId) const
{
	return true;
}


bool
AosBrowserThrd::doProcess()
{
	sgLock.lock();
	int idx = sgAccountIdx++;
	if (idx >= sgNumAccounts)
	{
		sgAccountIdx = 0;
	}
	sgLock.unlock();

	if(idx>4) idx=rand()%5;
	OmnString siteid = "100";
	OmnString username = sgUsernames[idx];
//	OmnScreen << username << endl;
	OmnString hpvpd = sgHpvpds[idx];
	OmnString login_vpdname = "yyy_login";
	OmnString ctnr = sgCtnrs[idx];
	OmnString passwd = sgPasswds[idx];
	OmnString ssid;
	OmnString objid = sgObjids[idx];
	
	//OmnScreen << "Thread: " << mThreadId << endl;
	
	bool rs = mTest->login(siteid, username, hpvpd, login_vpdname, ctnr, passwd, ssid);
	if(!rs) OmnScreen << "~~~~~~~~~~error~~~~~~~~~~~~~~~~~~~~~~~" << endl;

	return true;
}


