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
// 2013/07/01	Created by Ketty 
////////////////////////////////////////////////////////////////////////////
#include "RlbTester/IILClient/RlbIILClt.h"

#include "alarm_c/alarm.h"
#include "AppMgr/App.h"
#include "API/AosApi.h"
#include "Debug/Debug.h"
#include "Porting/Sleep.h"
#include "SEInterfaces/ProcInfo.h"
#include "RlbTester/Util/TesterCubeGrpInfo.h"
#include "Thread/Thread.h"
#include "Random/RandomUtil.h"
#include "Rundata/Rundata.h"
#include "Thread/Thread.h"
#include "TransBasic/Trans.h"
#include "Util/OmnNew.h"
#include "Util1/Time.h"
#include "Util/File.h"

#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <signal.h>

#include "IILTrans/HitAddDocByNameTrans.h"
#include "IILTrans/HitRemoveDocByNameTrans.h"
#include "IILTrans/HitRlbTesterCheckTrans.h"

const int sgIILNum = 2000;
const int sgMaxDocs = 10000000;
const int sgMaxStrLen = 80;

static char *       sgValues[sgMaxDocs];
static u64          sgDocids[sgMaxDocs];

AosRlbIILClt::AosRlbIILClt()
:
mLock(OmnNew OmnMutex()),
mDocid(10000),
mNumDocs(0),
mShowLog(true)
{
	for (int i=0; i<sgMaxDocs; i++)
	{
		sgDocids[i] = 0;
		sgValues[i] = new char[sgMaxStrLen+2];
	}
}


AosRlbIILClt::~AosRlbIILClt()
{
}


bool 
AosRlbIILClt::start()
{
	cout << "Start Reliable Tester ..." << endl;

	vector<u32> cube_grps;
	AosGetTotalCubeGrps(cube_grps);
	for(u32 i=0; i<cube_grps.size(); i++)
	{
		u32 cube_grp_id = cube_grps[i];
		
		AosTesterCubeGrpInfoPtr info = OmnNew AosTesterCubeGrpInfo(cube_grp_id, mShowLog);
		mCubeGrpInfo.insert(make_pair(cube_grp_id, info));
	}

	OmnThreadedObjPtr thisPtr(this, false);
	mMonitorThrd = OmnNew OmnThread(thisPtr, "RlbIILClt_MonitorThrd",
			eMonitorThrdId, true, true, __FILE__, __LINE__);
	//mMonitorThrd->start();

	AosRundataPtr rdata = OmnNew AosRundata(AosMemoryCheckerArgsBegin);
	rdata->setSiteid(1);

	while(1)
	{
		basicTest(rdata);
	}	
	return true;
}


bool
AosRlbIILClt::threadFunc(
		  OmnThrdStatus::E &state,
		  const OmnThreadPtr &thread)
{
	u32 tid = thread->getLogicId();
	
	if(tid == eMonitorThrdId)
	{
		return monitorThrdFunc(state, thread);
	}
	return true;
}


bool
AosRlbIILClt::monitorThrdFunc(
		  OmnThrdStatus::E &state,
		  const OmnThreadPtr &thread)
{
	bool rslt, rs;
	u32 sleep_tm;
	while(state == OmnThrdStatus::eActive)
	{
		// rand sleep 2-10 min.
		//sleep_tm = (rand() % 8 + 2) * 60;
		//sleep_tm = (rand() % 5 + 1) * 60;
		sleep_tm = (rand() % 8 + 2) * 60;
		OmnSleep(sleep_tm);
	
		AosTesterCubeGrpInfoPtr grp_info = randGetCubeGrpInfo();
		aos_assert_r(grp_info, false);
		
		grp_info->monitoyProc();
	}

	return true;
}


AosTesterCubeGrpInfoPtr
AosRlbIILClt::randGetCubeGrpInfo()
{
	aos_assert_r(mCubeGrpInfo.size(), 0);

	u32 idx = rand() % mCubeGrpInfo.size();
	mapitr_t itr = mCubeGrpInfo.begin();
	while(idx--) itr++;	
	aos_assert_r(itr != mCubeGrpInfo.end(), 0);

	return itr->second;
}


bool 
AosRlbIILClt::basicTest(const AosRundataPtr &rdata)
{
	// 1. random chose the app_id and pick the dfm
	// 2. random determain the opration
	bool rslt = false;
	switch (OmnRandom::percent(eAddWeight, eModifyWeight, eCheckWeight))
	{
	case 0:
		rslt = addEntry(rdata);
		break;
	
	case 1:
		rslt = removeEntry(rdata);
		break;

	case 2:
		rslt = check(rdata);
		break;
		
	default:
		OmnAlarm << "Invalid percent!" << enderr;
		return false;
	}
	return rslt;
}


bool
AosRlbIILClt::addEntry(const AosRundataPtr &rdata)
{
	if(mNumDocs > sgMaxDocs)	return true;
	
	OmnString iilname = createIILName();;
	
	u64 docid = mDocid;
	
	AosTransPtr trans = OmnNew AosIILTransHitAddDocByName(iilname, false, docid, true, false);
	bool svr_death;
	AosSendTrans(rdata, trans, svr_death);	
	if(svr_death)	return true;

	if(mShowLog)
	{
		OmnScreen << "RlbIILClt; addDoc"
			<< "; iilname:" << iilname 
			<< "; docid:" << docid 
			<< endl;
	}
	
	strcpy(sgValues[mNumDocs], iilname.data());
	sgDocids[mNumDocs] = mDocid;
	
	mNumDocs++;
	mDocid++;
	return true;
}


bool
AosRlbIILClt::removeEntry(const AosRundataPtr &rdata)
{
	if(mNumDocs == 0) return true;

	int idx = rand() % mNumDocs;
	u64 docid = sgDocids[idx];
	char * iilname = sgValues[idx];
	
	AosTransPtr trans = OmnNew AosIILTransHitRemoveDocByName(iilname, docid, true, false);
	
	bool svr_death;
	AosSendTrans(rdata, trans, svr_death);	
	if(svr_death)	return true;
	
	if(mShowLog)
	{
		OmnScreen << "RlbIILClt; removeDoc"
			<< "; iilname:" << iilname 
			<< "; docid:" << docid 
			<< endl;
	}

	if(idx < (mNumDocs - 1))
	{
		sgDocids[idx] = sgDocids[mNumDocs - 1];
		strcpy(sgValues[idx], sgValues[mNumDocs-1]);
	}
	
	sgDocids[mNumDocs-1] = 0;        
	strcpy(sgValues[mNumDocs-1], "");
	mNumDocs--;
	return true;
}


bool
AosRlbIILClt::check(const AosRundataPtr &rdata)
{
	if(mNumDocs == 0) return true;

	int idx = rand() % mNumDocs;
	u64 docid = sgDocids[idx];
	char * iilname = sgValues[idx];
	
	AosTransPtr trans = OmnNew AosIILTransHitRlbTesterCheck(iilname, docid);

	AosBuffPtr resp;
	bool svr_death;
	bool rslt = AosSendTrans(rdata, trans, resp, svr_death);
	aos_assert_r(rslt, false);
	if(svr_death)	return true;
	
	aos_assert_r(resp, false);
	bool exist = resp->getU8(0);
	if(!exist)
	{
		OmnAlarm << "RlbIILClt; check error!" 
			<< "; iilname:" << iilname
			<< "; docid:" << docid 
			<< enderr;
	}
	return true;
}


OmnString
AosRlbIILClt::createIILName()
{
	int i = rand() % sgIILNum + 1;
	OmnString word = "tester";
	word << i;
	return word;
}


