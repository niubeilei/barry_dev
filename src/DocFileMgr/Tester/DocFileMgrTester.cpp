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
#include "DocFileMgr/Tester/DocFileMgrTester.h"

#include "alarm_c/alarm.h"
#include "AppMgr/App.h"
#include "Debug/Debug.h"
#include "DfmUtil/DfmDocNorm.h"
#include "DocFileMgr/Tester/DfmInfo.h"
#include "DocFileMgr/Tester/DocInfo.h"
#include "DocFileMgr/Tester/SnapInfo.h"
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

AosDocFileMgrTester::AosDocFileMgrTester()
:
mLock(OmnNew OmnMutex()),
mCondVar(OmnNew OmnCondVar),
//mDfmConf(AosDfmDocType::eNormal, "Data", AosModuleId::eDoc,
//		false, eSnapNum, "gzip", true)
mDfmConf(AosDfmDocType::eDatalet, "Data", AosModuleId::eDoc,
		false, eSnapNum, "gzip", true)
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

	// start thread
	OmnThreadedObjPtr thisPtr(this, false);
	OmnThreadPtr thread[eThreadNum];
	for(u32 i=0; i<eThreadNum; i++)
	{
		thread[i] = OmnNew OmnThread(thisPtr, "Thrd", i, true, true, __FILE__, __LINE__);
		thread[i]->start();
		mThrdStoped[i] = false;
	}

	OmnThreadPtr kill_thrd = OmnNew OmnThread(thisPtr, "Thrd", eThreadNum, true, true, __FILE__, __LINE__);
	kill_thrd->start();

	while(1)
	{
		OmnSleep(1 * 60);
		//OmnSleep(1 * 10);

		snapShotTest();
	}

	return true;
}


void
AosDocFileMgrTester::init()
{
	mRdata = OmnNew AosRundata(AosMemoryCheckerArgsBegin);
	mRdata->setSiteid(100);
	
	OmnString fname = "Dfm_Tester_Data";
	mFile = OmnNew OmnFile(fname, OmnFile::eReadWrite AosMemoryCheckerArgs);
	if (!mFile->isGood())
	{
		mFile = OmnNew OmnFile(fname, OmnFile::eCreate AosMemoryCheckerArgs);
	}
	aos_assert(mFile && mFile->isGood());
	
	u32 buff_len = mFile->getLength();
	if(buff_len == 0)
	{
		mDfmInfo = OmnNew AosDfmInfo(mRdata, mDfmConf);
		AosDocInfo::staticInit();
		return; 
	}
		
	AosBuffPtr buff = OmnNew AosBuff(buff_len, 0 AosMemoryCheckerArgs);
	mFile->readToBuff(buff, buff_len + 1);

	mDfmInfo = OmnNew AosDfmInfo(mRdata, mDfmConf);
	bool rslt = mDfmInfo->serializeFrom(buff);
	aos_assert(rslt);
	
	AosDocInfo::staticInit();
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
		return killThrdFunc(state, thread);
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

	//thread->stop();
	return true;
}


bool
AosDocFileMgrTester::killThrdFunc(
		  OmnThrdStatus::E &state,
		  const OmnThreadPtr &thread)
{
	bool timeout = false;

	while(state == OmnThrdStatus::eActive)
	{
		mLock->lock();
		mCondVar->timedWait(mLock, timeout, 1);
		mLock->unlock();
		
		if(timeout)	continue;

		// prepair to kill self.
		u32 sleep_time = rand() % 10000;
		OmnMsSleep(sleep_time);
		kill(getpid(), 9);
		//OmnAlarm << "error" << enderr;
	}
	return true;
}


bool 
AosDocFileMgrTester::basicTest()
{
	// 1. random chose the app_id and pick the dfm
	// 2. random determain the opration
	u32 flag = (rand() % 2);
	if(flag == 0)	return normProc();

	return snapProc();
}


bool
AosDocFileMgrTester::normProc()
{
	bool rslt = false;
	switch (OmnRandom::percent(eAddWeight, eModifyWeight, eDeleteWeight, eReadWeight))
	{
	case 0:
		rslt = mDfmInfo->addDoc(mRdata);
		break;
	
	case 1:
		rslt = mDfmInfo->randModifyDoc(mRdata);
		break;

	case 2:
		rslt = mDfmInfo->randDeleteDoc(mRdata);
		break;
		
	case 3:
		rslt = mDfmInfo->randReadDoc(mRdata);
		break;

	default:
		OmnAlarm << "Invalid percent!" << enderr;
		return false;
	}
	
	return rslt;
}

	
bool
AosDocFileMgrTester::snapProc()
{
	bool rslt = false;
	switch (OmnRandom::percent(eAddWeight, eModifyWeight, eDeleteWeight, eReadWeight))
	{
	case 0:
		rslt = mDfmInfo->addDocToSnap(mRdata);
		rslt = mDfmInfo->addDoc(mRdata);
		break;
	
	case 1:
		rslt = mDfmInfo->randModifyDocFromSnap(mRdata);
		break;

	case 2:
		rslt = mDfmInfo->randDeleteDocFromSnap(mRdata);
		break;
		
	case 3:
		rslt = mDfmInfo->randReadDocFromSnap(mRdata);
		break;

	default:
		OmnAlarm << "Invalid percent!" << enderr;
		return false;
	}
	
	return rslt;
}


bool
AosDocFileMgrTester::snapShotTest()
{
	bool rslt = false;
	
	u32 rr = OmnRandom::percent(eAddSnapWeight, eRollbackWeight, eCommitWeight, eMergeWeight);
	switch (rr)
	{
	case 0:
		rslt = addSnapShot(mRdata);
		break;
	
	case 1:
		rslt = rollback(mRdata);
		break;
		
	case 2:
		rslt = commit(mRdata);
		break;

	case 3:
		rslt = merge(mRdata);
		break;

	default:
		OmnAlarm << "Invalid percent!" << enderr;
		return false;
	}
	return rslt;
}


bool
AosDocFileMgrTester::addSnapShot(const AosRundataPtr &rdata)
{
	if(!mDfmInfo->hasMemory())	return true;
	
	OmnScreen << "start add Snapshot" << endl;
	mDfmInfo->snapWriteLock();

	AosDocFileMgrObjPtr dfm = mDfmInfo->getDocFileMgr();
	aos_assert_r(dfm, false);

	AosTransId trans_id;
	u64 dfm_snap_id = dfm->createSnapshot(rdata, trans_id);
	aos_assert_r(dfm_snap_id, false);

	bool rslt = mDfmInfo->addSnapShot(dfm_snap_id);
	aos_assert_r(rslt, false);
		
	mDfmInfo->snapUnLock();
	
	OmnScreen << "add Snapshot"
		<< "; snap_id:" << dfm_snap_id
		<< endl;
}


bool
AosDocFileMgrTester::rollback(const AosRundataPtr &rdata)
{
	AosSnapInfoPtr snap = mDfmInfo->randGetSnap();
	if(!snap) return true;	
	
	bool kill = needKill();	
	if(kill)	readyKill();

	OmnScreen << "start rollback Snapshot; " << endl;
	
	mDfmInfo->snapWriteLock();
	bool rslt = mDfmInfo->rollback(snap);
	aos_assert_r(rslt, false);

	if(kill)	saveToFile();

	AosDocFileMgrObjPtr dfm = mDfmInfo->getDocFileMgr();
	aos_assert_r(dfm, false);
	AosTransId trans_id;
	rslt = dfm->rollbackSnapshot(rdata, snap->getSnapId(), trans_id);
	aos_assert_r(rslt, false);

	rslt = mDfmInfo->sanitycheck(rdata);
	aos_assert_r(rslt, false);

	mDfmInfo->snapUnLock();

	OmnScreen << "rollBack Snapshot"        
		<< "; snap_id:" << snap->getSnapId()
		<< endl;

	return true;
}


bool
AosDocFileMgrTester::commit(const AosRundataPtr &rdata)
{
	AosSnapInfoPtr snap = mDfmInfo->randGetSnap();
	if(!snap) return true;	

	bool kill = needKill();	
	if(kill)	readyKill();
	
	OmnScreen << "start commit Snapshot; " << endl;

	mDfmInfo->snapWriteLock();
	bool rslt = mDfmInfo->commit(snap);
	aos_assert_r(rslt, false);

	if(kill)	saveToFile();

	AosDocFileMgrObjPtr dfm = mDfmInfo->getDocFileMgr();
	aos_assert_r(dfm, false);
	
	AosTransId trans_id;
	rslt = dfm->commitSnapshot(rdata, snap->getSnapId(), trans_id);
	aos_assert_r(rslt, false);
	
	rslt = mDfmInfo->sanitycheck(rdata);
	aos_assert_r(rslt, false);
	
	mDfmInfo->snapUnLock();
	
	OmnScreen << "commit Snapshot; "
		<< "; snap_id:" << snap->getSnapId()
		<< endl;
	
	return true;
}


bool
AosDocFileMgrTester::merge(const AosRundataPtr &rdata)
{
	if(mDfmInfo->getSnapNum() <=1)	return true;
	
	AosSnapInfoPtr target_snap = mDfmInfo->randGetSnap();
	if(!target_snap) return true;	
	
	AosSnapInfoPtr merger_snap;
	while(1)
	{
		merger_snap = mDfmInfo->randGetSnap();
		if(!merger_snap) return true;

		if(merger_snap->getSnapId() != target_snap->getSnapId())	break;
	}
	
	OmnScreen << "start merge Snapshot" << endl;
	
	mDfmInfo->snapWriteLock();
	
	bool rslt = mDfmInfo->merge(target_snap, merger_snap);
	aos_assert_r(rslt, false);

	AosDocFileMgrObjPtr dfm = mDfmInfo->getDocFileMgr();
	aos_assert_r(dfm, false);
	
	AosTransId trans_id;
	rslt = dfm->mergeSnapshot(rdata, target_snap->getSnapId(),
			merger_snap->getSnapId(), trans_id);
	aos_assert_r(rslt, false);
	
	//rslt = mDfmInfo->sanitycheck(rdata);
	//aos_assert_r(rslt, false);
	
	mDfmInfo->snapUnLock();
	
	OmnScreen << "merge Snapshot; "
		<< "; target_snap_id:" << target_snap->getSnapId()
		<< "; merger_snap_id:" << merger_snap->getSnapId()
		<< endl;
	
	return true;
}


bool
AosDocFileMgrTester::needKill()
{
	return false;
	u32 val = (rand() % 2);
	return val == 0;
}


bool
AosDocFileMgrTester::readyKill()
{
	// first save to file.
	AosBuffPtr dfm_buff = OmnNew AosBuff(1000, 0 AosMemoryCheckerArgs);
	
	mStop = true;
	OmnSleep(2);	// wait all basicThrd stop.

	for(u32 i=0; i<eThreadNum; i++)
	{
		if(mThrdStoped[i])	continue;
		mDfmInfo->snapUnLock();
		OmnSleep(1);
		i--;
	}
	
	return true;
}


bool
AosDocFileMgrTester::saveToFile()
{
	OmnScreen << "AosDocFileMgrTester:: saveToFile start!" << endl;
	
	AosBuffPtr dfm_buff = OmnNew AosBuff(1000, 0 AosMemoryCheckerArgs);
	mDfmInfo->serializeTo(dfm_buff);
	mFile->put(0, dfm_buff->data(), dfm_buff->dataLen(), true); 
	
	OmnScreen << "AosDocFileMgrTester:: saveToFile end!" << endl;

	mLock->lock();
	mCondVar->signal();
	mLock->unlock();
	
	return true;
}
