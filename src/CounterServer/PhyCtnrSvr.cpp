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
// All requests are stored in mRequests. When there are too many, it will 
// stop adding requests to mRequests. When mRequests are empty, it checks
// whether there are additional requests from the log file. If yes, it
// reads in all the requests to mRequests. 
//
// Modification History:
// 05/19/211: Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "CounterServer/PhyCtnrSvr.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "API/AosApi.h"
#include "AppMgr/App.h"
#include "Thread/Mutex.h"
#include "Thread/Thread.h"
#include "CounterServer/Ptrs.h"
#include "CounterServer/VirCtnrSvr.h"
#include "TransServer/Ptrs.h"
#include "Thread/ThreadPool.h"
#include <set>

using namespace std;


extern int gAosLogLevel;

static OmnThreadPoolPtr sgThreadPool = OmnNew OmnThreadPool("PhyCtnrSvr", __FILE__, __LINE__);
static AosVirCtnrSvrPtr	*sgVirtualServers = NULL;

AosXmlTagPtr AosPhyCtnrSvr::mVirConfig = 0;
int   AosPhyCtnrSvr::mNumVirtuals = 0 ;

/*
AosPhyCtnrSvr::AosPhyCtnrSvr(const AosXmlTagPtr &theconfig)
{
	OmnScreen << "Starting the physics server ...." << endl;
	//aos_assert(config(theconfig));
}
*/

AosPhyCtnrSvr::AosPhyCtnrSvr()
{
	OmnScreen << "Starting the physics server ...." << endl;

}

AosPhyCtnrSvr::~AosPhyCtnrSvr()
{
	OmnDelete [] sgVirtualServers;
}


bool
AosPhyCtnrSvr::resetVirtualServers()
{
	return true;
}


bool
AosPhyCtnrSvr::config(const AosXmlTagPtr &config)
{
	aos_assert_r(config, false);
	AosXmlTagPtr serverConfig = config->getFirstChild(AOSTAG_COUNTERSVR);
	aos_assert_r(serverConfig, false);
	mNumVirtuals = AosGetNumCubes();
	aos_assert_r(mNumVirtuals > 0, false);

	//mVirConfig = serverConfig->getFirstChild(AOSTAG_VIRTUALCFG);
	sgVirtualServers = OmnNew AosVirCtnrSvrPtr[mNumVirtuals];

	bool showlog = serverConfig->getAttrBool(AOSCONFIG_SHOWLOG, false);
	AosVirCtnrSvr::setShowLog(showlog);

	return true;
}

bool
AosPhyCtnrSvr::start()
{
	
	return true;
}


/*
bool
AosPhyCtnrSvr::proc(
		const AosTransPtr &trans,
		const AosRundataPtr &rdata)
{
	// This is a batch transaction that contains many transactions.
	aos_assert_rr(trans, rdata, false);
//OmnScreen << "To proc trans" << endl;

	 u64 dist_id = trans->getDistId();
	 aos_assert_rr(mNumVirtuals > 0, rdata, false);
	 u32 vid = dist_id % mNumVirtuals;
	 AosVirCtnrSvrPtr vserver = getVirtualServer(vid);
	 aos_assert_rr(vserver, rdata, false);
	 //return vserver->proc(trans, rdata);
	 OmnThrdShellProcPtr runner = OmnNew procThrd(vserver, trans, rdata);
//	 return OmnThreadShellMgr::getSelf()->proc(runner);
}
*/

	
bool
AosPhyCtnrSvr::addTrans(
		const AosCounterTransPtr &trans,
		const AosRundataPtr &rdata)
{
	aos_assert_r(trans, false);
	u32 vid = trans->getCubeId();
	AosVirCtnrSvrPtr vserver = getVirtualServer(vid);
	aos_assert_r(vserver, false);
	 
	OmnThrdShellProcPtr runner = OmnNew procThrd(vserver, trans, rdata);
	aos_assert_r(sgThreadPool, false);
	return sgThreadPool->proc(runner);
}


AosVirCtnrSvrPtr
AosPhyCtnrSvr::getVirtualServer(const u32 id)
{
	if(id >=0 && id < (u32)mNumVirtuals)
	{
		if (!sgVirtualServers[id])
			sgVirtualServers[id] = OmnNew AosVirCtnrSvr(id, mVirConfig);
		return sgVirtualServers[id];
	}
	else
	{
		OmnAlarm << "Cann't get VirtualServer: " << id << enderr;
		return 0;
	}
}


bool
AosPhyCtnrSvr::stop()
{
	for(int i = 0; i < mNumVirtuals; i++)
	{
		bool rslt = sgVirtualServers[i]->stop();
		aos_assert_r(rslt, false);
	}
	return true;
}


bool
AosPhyCtnrSvr::procThrd::run()
{
	aos_assert_r(mVserver && mRdata && mTrans, false);
	
	return mVserver->proc(mTrans, mRdata);
}


bool
AosPhyCtnrSvr::procThrd::procFinished()
{
	return true;
}

