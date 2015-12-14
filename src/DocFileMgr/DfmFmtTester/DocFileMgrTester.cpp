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
// 2010/10/24	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "DocFileMgr/DfmFmtTester/DocFileMgrTester.h"

#include "alarm_c/alarm.h"
#include "AppMgr/App.h"
#include "Debug/Debug.h"
#include "DfmUtil/DfmDocNorm.h"
#include "DocFileMgr/DfmFmtTester/DfmInfo.h"
#include "DocFileMgr/DfmFmtTester/TesterCubeGrpInfo.h"
#include "Porting/Sleep.h"
#include "SEInterfaces/DocFileMgrObj.h"
#include "Tester/Test.h"
#include "Tester/TestMgr.h"
#include "Random/RandomUtil.h"
#include "Rundata/Rundata.h"
#include "Thread/Thread.h"
#include "Util/OmnNew.h"
#include "Util1/Time.h"
#include "Util/File.h"

#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <signal.h>

bool	mStop = false;
static 	bool smShowLog = true;


AosDocFileMgrTester::AosDocFileMgrTester()
:
mLock(OmnNew OmnMutex()),
mCondVar(OmnNew OmnCondVar),
mCrtMaxDocid(AosGetNumCubes())
{
}


AosDocFileMgrTester::~AosDocFileMgrTester()
{
}


bool 
AosDocFileMgrTester::start()
{
	cout << "Start AosDocFileMgr Tester ..." << endl;
	init();
	
	vector<u32> cube_grps;
	AosGetTotalCubeGrps(cube_grps);
	for(u32 i=0; i<cube_grps.size(); i++)
	{
		u32 cube_grp_id = cube_grps[i];
		
		AosTesterCubeGrpInfoPtr info = OmnNew AosTesterCubeGrpInfo(cube_grp_id, smShowLog);
		mCubeGrpInfo.insert(make_pair(cube_grp_id, info));
	}

	// start thread
	OmnThreadedObjPtr thisPtr(this, false);
	for(u32 i=0; i<eThreadNum; i++)
	{
		OmnString str = "RlbTester_BasicThrd";
		str << "_" << i;
		mBasicThrds[i] = OmnNew OmnThread(thisPtr, str,
				i, true, true, __FILE__, __LINE__);
		mBasicThrds[i]->start();
		mThrdStoped[i] = false;
	}

	mMonitorThrd = OmnNew OmnThread(thisPtr, "RlbTester_MonitorThrd",
			eThreadNum, true, true, __FILE__, __LINE__);
	mMonitorThrd->start();
	return true;
}


void
AosDocFileMgrTester::init()
{
	AosRundataPtr rdata = OmnNew AosRundata(AosMemoryCheckerArgsBegin);
	int v_num = AosGetNumCubes();
	aos_assert(v_num > 0);

	bool rslt;
	AosDfmInfoPtr dfm_info;
	for(u32 site_id = eSiteStart; site_id<= 100; site_id++)
	{
		rdata->setSiteid(site_id);
		for(u32 vid=0; vid<v_num; vid++)
		{
			u64 dfm_info_id = vid;
			dfm_info_id = (dfm_info_id << 32) + site_id;
			
			dfm_info = OmnNew AosDfmInfo(rdata, vid, smShowLog);
			mDfmInfo.insert(make_pair(dfm_info_id, dfm_info));
		}
	}

	AosDocInfo3::staticInit();
}


bool
AosDocFileMgrTester::threadFunc(
		  OmnThrdStatus::E &state,
		  const OmnThreadPtr &thread)
{
	u32 tid = thread->getLogicId();
	
	if(tid >=0 && tid < eThreadNum)
	{
		return basicThrdFunc(state, thread);
	}

	if(tid == eThreadNum)
	{
		//return killThrdFunc(state, thread);
		return monitorThrdFunc(state, thread);
	}
	return true;
}


bool
AosDocFileMgrTester::basicThrdFunc(
		  OmnThrdStatus::E &state,
		  const OmnThreadPtr &thread)
{
	while(state == OmnThrdStatus::eActive)
	{
		if(mStop) break;
		basicTest();
	}
	
	u32 tid = thread->getLogicId();
	aos_assert_r(tid < eThreadNum, false);
	mThrdStoped[tid] = true;
	
	OmnScreen << "Thread:" << thread->getLogicId() 
		<< "; Existing!	" 
		<< "Total Num:" << eThreadNum 
		<< endl;

	return true;
}


bool
AosDocFileMgrTester::monitorThrdFunc(
		  OmnThrdStatus::E &state,
		  const OmnThreadPtr &thread)
{
	bool rslt, rs;
	u32 sleep_tm;
	while(state == OmnThrdStatus::eActive)
	{
		// rand sleep 2-10 min.
		//sleep_tm = (rand() % 8 + 2) * 60;
		sleep_tm = (rand() % 5 + 5) * 60;
		OmnSleep(sleep_tm);
	
		AosTesterCubeGrpInfoPtr grp_info = randGetCubeGrpInfo();
		aos_assert_r(grp_info, false);
		
		grp_info->monitoyProc();
	}

	return true;
}


AosTesterCubeGrpInfoPtr
AosDocFileMgrTester::randGetCubeGrpInfo()
{
	aos_assert_r(mCubeGrpInfo.size(), 0);

	u32 idx = rand() % mCubeGrpInfo.size();
	map<u32, AosTesterCubeGrpInfoPtr>::iterator itr = mCubeGrpInfo.begin();
	while(idx--) itr++;	
	aos_assert_r(itr != mCubeGrpInfo.end(), 0);

	return itr->second;
}


bool 
AosDocFileMgrTester::basicTest()
{
	// 1. random chose the app_id and pick the dfm
	AosRundataPtr rdata = OmnNew AosRundata(AosMemoryCheckerArgsBegin);
	u32 site_id = rand() % eSiteNum + eSiteStart;
	rdata->setSiteid(site_id);

	// 2. random determain the opration
	bool rslt = false;
	switch (OmnRandom::percent(eCreateWeight, eModifyWeight, eDeleteWeight, eReadWeight))
	{
	case 0:
		rslt = createDoc(rdata);
		break;
	
	case 1:
		rslt = modifyDoc(rdata);
		break;

	case 2:
		rslt = deleteDoc(rdata);
		break;
		
	case 3:
		readDoc(rdata);
		break;

	default:
		OmnAlarm << "Invalid percent!" << enderr;
		return false;
	}
	return rslt;
}


bool
AosDocFileMgrTester::createDoc(const AosRundataPtr &rdata)
{
	mLock->lock();
	u64 docid = mCrtMaxDocid++; 
	mLock->unlock();
	
	AosDfmInfoPtr dfm_info = getDfmInfo(docid);
	AosDocInfo3Ptr doc_info = dfm_info->addDoc(rdata, docid);
	if(!doc_info)
	{
		//mLock->lock();
		//mCrtMaxDocid--;
		//mLock->unlock();
		return true;
	}

	return true;
}


bool
AosDocFileMgrTester::modifyDoc(const AosRundataPtr &rdata)
{
	AosDfmInfoPtr dfm_info = randGetDfmInfo();
	if(!dfm_info)	return true;

	bool rslt = dfm_info->randModifyDoc(rdata);
	aos_assert_r(rslt, false);

	return true;
}


bool
AosDocFileMgrTester::deleteDoc(const AosRundataPtr &rdata)
{
	AosDfmInfoPtr dfm_info = randGetDfmInfo();
	if(!dfm_info)	return true;

	bool rslt = dfm_info->randDeleteDoc(rdata);
	aos_assert_r(rslt, false);

	return true;
}


bool
AosDocFileMgrTester::readDoc(const AosRundataPtr &rdata)
{
	AosDfmInfoPtr dfm_info = randGetDfmInfo();
	if(!dfm_info)	return true;

	bool rslt = dfm_info->randReadDoc(rdata);
	aos_assert_r(rslt, false);

	return true;
}


AosDfmInfoPtr
AosDocFileMgrTester::randGetDfmInfo()
{
	if(mDfmInfo.size() == 0)	return 0;
	u32 dfm_idx =  rand() % mDfmInfo.size();	

	map<u64, AosDfmInfoPtr>::iterator itr = mDfmInfo.begin();
	for(u32 i=0; i<dfm_idx; i++)    itr++;

	AosDfmInfoPtr dfm_info = itr->second;
	return dfm_info;
}


AosDfmInfoPtr
AosDocFileMgrTester::getDfmInfo(const u64 docid)
{
	if(mDfmInfo.size() == 0)	return 0;
	
	u32 vid = AosGetCubeId(docid); 
	u64 dfm_info_id = vid;
	
	u32 site_id = rand() % eSiteNum + eSiteStart;
	dfm_info_id = (dfm_info_id << 32) + site_id;
	
	map<u64, AosDfmInfoPtr>::iterator itr = mDfmInfo.find(dfm_info_id);
	if(itr == mDfmInfo.end())	return 0;

	AosDfmInfoPtr dfm_info = itr->second;
	return dfm_info;
}


bool
AosDocFileMgrTester::saveToFile()
{
	OmnScreen << "AosDocFileMgrTester:: saveToFile start!" << endl;
	
	AosBuffPtr dfm_buff = OmnNew AosBuff(1000, 0 AosMemoryCheckerArgs);
	
	dfm_buff->setU32(mDfmInfo.size());
	map<u64, AosDfmInfoPtr>::iterator itr = mDfmInfo.begin();
	for(; itr != mDfmInfo.end(); itr++)
	{
		AosDfmInfoPtr dfm_info = itr->second;
		dfm_info->serializeTo(dfm_buff);
	}
		
	mFile->put(0, dfm_buff->data(), dfm_buff->dataLen(), true); 
	
	OmnScreen << "AosDocFileMgrTester:: saveToFile end!" << endl;
	return true;
}

