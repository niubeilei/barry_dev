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
#include "DocFileMgr/DfmLogTester/DocFileMgrTester.h"

#include "alarm_c/alarm.h"
#include "AppMgr/App.h"
#include "Debug/Debug.h"
#include "DfmUtil/DfmDocNorm.h"
#include "DocFileMgr/DfmLogTester/DfmInfo.h"
#include "DocFileMgr/DfmLogTester/DocInfo.h"
#include "DocFileMgr/DfmLogTester/SnapInfo.h"
#include "FmtMgr/FmtMgr.h"
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
mDfmConf(AosDfmDoc::eNormal, (AosDfmType::E)eDfmType, 
		"Data", AosModuleId::eDoc, eSnapNum, "gzip", false),
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

//	while(1)
//	{
//		OmnSleep(1 * 60);
//		//OmnSleep(1 * 10);
//
//		snapShotTest();
//	}
//
	return true;
}


void
AosDocFileMgrTester::init()
{
	AosRundataPtr rdata = OmnNew AosRundata(AosMemoryCheckerArgsBegin);
	
	OmnString fname = "./Dfm_Tester_Data";
	mFile = OmnNew OmnFile(fname, OmnFile::eReadWrite AosMemoryCheckerArgs);
	if (!mFile->isGood())
	{
		mFile = OmnNew OmnFile(fname, OmnFile::eCreate AosMemoryCheckerArgs);
	}
	aos_assert(mFile && mFile->isGood());

	u32 buff_len = mFile->getLength();
	AosBuffPtr buff = OmnNew AosBuff(buff_len, 0 AosMemoryCheckerArgs);
	if(buff_len)
	{
		mFile->readToBuff(buff, buff_len + 1);
	}

	u64 max_docid = 0;
	bool rslt;
	AosDfmInfoPtr dfm_info;
	u32 dfm_info_num = buff->getU32(0);
	for(int i=0; i<dfm_info_num; i++)
	{
		dfm_info = OmnNew AosDfmInfo();
		rslt = dfm_info->serializeFrom(buff, mDfmConf);
		aos_assert(rslt);

		u64 dfm_info_id = dfm_info->getDfmInfoId();
		mDfmInfo.insert(make_pair(dfm_info_id, dfm_info));

		u64 crt_max = dfm_info->getMaxDocid();
		if(crt_max > max_docid)	max_docid = crt_max;
	}
	
	max_docid += 1;
	if(max_docid > mCrtMaxDocid)	mCrtMaxDocid = max_docid;

	map<u64, AosDfmInfoPtr>::iterator itr;
	int v_num = AosGetNumCubes();
	aos_assert(v_num > 0);
	//for(u32 site_id=1; site_id<= eSiteNum; site_id++)
	for(u32 site_id=100; site_id<= 100; site_id++)
	{
		rdata->setSiteid(site_id);
		for(u32 vid=0; vid<v_num; vid++)
		{
			u64 dfm_info_id = vid;
			dfm_info_id = (dfm_info_id << 32) + site_id;
			
			itr = mDfmInfo.find(dfm_info_id);
			if(itr != mDfmInfo.end())	continue;

			dfm_info = OmnNew AosDfmInfo(rdata, vid, mDfmConf);
			mDfmInfo.insert(make_pair(dfm_info_id, dfm_info));
		}
	}

	
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
	
	//OmnScreen << "Thread:" << thread->getLogicId() 
	//	<< "; Existing!	" 
	//	<< "Total Num:" << eThreadNum 
	//	<< endl;

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
		mCondVar->timedWait(mLock, timeout, 5);
		mLock->unlock();
		if (mStop) break;
		
		if(timeout)	continue;

		// prepair to kill self.
		u32 sleep_time = rand() % 10000;
		OmnMsSleep(sleep_time);
mStop = true;
		OmnSleep(10);
saveToFile();
		kill(getpid(), 14);
		//OmnAlarm << "error" << enderr;
	}
	return true;
}


bool 
AosDocFileMgrTester::basicTest()
{
	// 1. random chose the app_id and pick the dfm
	AosRundataPtr rdata = OmnNew AosRundata(AosMemoryCheckerArgsBegin);
	u32 site_id = rand() % eSiteNum +100;
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
	
	AosDfmInfoPtr dfm_info = getDfmInfo(docid);
	AosDocInfoPtr doc_info = dfm_info->addDoc(rdata, docid);
	if(!doc_info)
	{
		mLock->lock();
		mCrtMaxDocid--;
		mLock->unlock();

		return true;
	}
	mLock->unlock();

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
	
	u32 site_id = rand() % eSiteNum +100;
	dfm_info_id = (dfm_info_id << 32) + site_id;
	
	map<u64, AosDfmInfoPtr>::iterator itr = mDfmInfo.find(dfm_info_id);
	if(itr == mDfmInfo.end())	return 0;

	AosDfmInfoPtr dfm_info = itr->second;
	return dfm_info;
}


bool
AosDocFileMgrTester::snapShotTest()
{
	// 1. random chose the app_id and pick the dfm
	AosRundataPtr rdata = OmnNew AosRundata(AosMemoryCheckerArgsBegin);
	u32 site_id = rand() % eSiteNum +100;
	rdata->setSiteid(site_id);

	// 2. random determain the dfm.
	AosDfmInfoPtr dfm_info = randGetDfmInfo();
	if(!dfm_info)	return true;

	// 3. random determain the snap opration
	bool rslt = false;
	switch (OmnRandom::percent(eAddSnapWeight, eRmSnapWeight, eRbWeight, eRbWeight))
	{
	case 0:
		rslt = addSnapShot(rdata, dfm_info);
		break;
	
	case 1:
		rslt = removeSnapShot(rdata, dfm_info);
		break;

	case 2:
		rslt = rollBack(rdata, dfm_info);
		break;
		
	case 3:
		commit(rdata, dfm_info);
		break;

	default:
		OmnAlarm << "Invalid percent!" << enderr;
		return false;
	}
	return rslt;
}


/*
bool
AosDocFileMgrTester::addSnapShot(
		const AosRundataPtr &rdata,
		const AosDfmInfoPtr &dfm_info)
{
	if(!dfm_info->canAddSnap())	return true;
	
	//readyKillSelf();
	bool rslt = dfm_info->addSnapShot(rdata);
	return true;
}


bool
AosDocFileMgrTester::removeSnapShot(
		const AosRundataPtr &rdata,
		const AosDfmInfoPtr &dfm_info)
{
	if(dfm_info->getSnapNum() == 0)	return true;
	
	bool kill = readyKillSelf();
	bool rslt = dfm_info->randRemoveSnapShot(rdata, kill);
	return true;
}

bool
AosDocFileMgrTester::rollBack(
		const AosRundataPtr &rdata,
		const AosDfmInfoPtr &dfm_info)
{
	if(dfm_info->getSnapNum() == 0)	return true;
	
	bool kill = readyKillSelf();
	bool rslt = dfm_info->randRollBack(rdata, kill);
	return true;
}

bool
AosDocFileMgrTester::commit(
		const AosRundataPtr &rdata,
		const AosDfmInfoPtr &dfm_info)
{
	if(dfm_info->getSnapNum() == 0)	return true;
	
	bool kill = readyKillSelf();
	bool rslt = dfm_info->commit(rdata, kill);
	return true;
}
*/


bool
AosDocFileMgrTester::addSnapShot(
		const AosRundataPtr &rdata,
		const AosDfmInfoPtr &dfm_info)
{
	if(!dfm_info->canAddSnap())	return true;
	
	dfm_info->snapWriteLock();

	u32 dfm_snap_id = 0;
	AosDocFileMgrObjPtr dfm = dfm_info->getDocFileMgr();
	aos_assert_r(dfm, false);
	bool rslt = dfm->addSnapShot(rdata, dfm_snap_id);
	aos_assert_r(rslt, false);

	rslt = dfm_info->addSnapShot(dfm_snap_id);
	aos_assert_r(rslt, false);
		
	dfm_info->snapUnLock();
	
	OmnScreen << "add SnapShot"
		<< "; virtual_id:" << dfm_info->getVirtualId() 
		<< "; dfmId:" << dfm_info->getDfmId()
		<< "; snap_id:" << dfm_snap_id
		<< endl;
}


bool
AosDocFileMgrTester::removeSnapShot(
		const AosRundataPtr &rdata,
		const AosDfmInfoPtr &dfm_info)
{
	AosSnapInfoPtr snap = dfm_info->randGetSnap();
	if(!snap) return true;	
	
	bool kill = needKill();	
	if(kill)	readyKill(dfm_info);

	dfm_info->snapWriteLock();
	bool rslt = dfm_info->removeSnapShot(snap);
	aos_assert_r(rslt, false);

	if(kill)	saveToFile();
	
	AosDocFileMgrObjPtr dfm = dfm_info->getDocFileMgr();
	aos_assert_r(dfm, false);
	dfm->removeSnapShot(rdata, snap->getSnapId());
	aos_assert_r(rslt, false);

	dfm_info->snapUnLock();
	
	OmnScreen << "remove SnapShot"
		<< "; virtual_id:" << dfm_info->getVirtualId() 
		<< "; dfmId:" << dfm_info->getDfmId()
		<< "; snap_id:" << snap->getSnapId()
		<< endl;
	
	return true;
}


bool
AosDocFileMgrTester::rollBack(
		const AosRundataPtr &rdata,
		const AosDfmInfoPtr &dfm_info)
{
	AosSnapInfoPtr snap = dfm_info->randGetSnap();
	if(!snap) return true;	
	
	bool kill = needKill();	
	if(kill)	readyKill(dfm_info);

	dfm_info->snapWriteLock();
	bool rslt = dfm_info->rollBack(snap);
	aos_assert_r(rslt, false);

	if(kill)	saveToFile();

	AosDocFileMgrObjPtr dfm = dfm_info->getDocFileMgr();
	aos_assert_r(dfm, false);
	dfm->rollBack(rdata, snap->getSnapId());
	aos_assert_r(rslt, false);

	rslt = dfm_info->checkRollBack(rdata);
	aos_assert_r(rslt, false);

	dfm_info->snapUnLock();

	OmnScreen << "rollBack SnapShot"        
		<< "; virtual_id:" << dfm_info->getVirtualId() 
		<< "; dfmId:" << dfm_info->getDfmId()
		<< "; snap_id:" << snap->getSnapId()
		<< endl;

	return true;
}


bool
AosDocFileMgrTester::commit(
		const AosRundataPtr &rdata,
		const AosDfmInfoPtr &dfm_info)
{
	if(dfm_info->getSnapNum() == 0)	return true;

	bool kill = needKill();	
	if(kill)	readyKill(dfm_info);

	dfm_info->snapWriteLock();
	bool rslt = dfm_info->commit();
	aos_assert_r(rslt, false);

	if(kill)	saveToFile();

	AosDocFileMgrObjPtr dfm = dfm_info->getDocFileMgr();
	aos_assert_r(dfm, false);
	dfm->commit(rdata);
	aos_assert_r(rslt, false);
	
	dfm_info->snapUnLock();
	
	OmnScreen << "commit SnapShot"
		<< "; virtual_id:" << dfm_info->getVirtualId() 
		<< "; dfmId:" << dfm_info->getDfmId()
		<< endl;
	
	return true;
}
	

bool
AosDocFileMgrTester::needKill()
{
	u32 val = (rand() % 2);
	return val == 0;
}


bool
AosDocFileMgrTester::readyKill(const AosDfmInfoPtr &dfm_info)
{
	// first save to file.
	AosBuffPtr dfm_buff = OmnNew AosBuff(1000, 0 AosMemoryCheckerArgs);
	
	mStop = true;
	OmnSleep(2);	// wait all basicThrd stop.

	for(u32 i=0; i<eThreadNum; i++)
	{
		if(mThrdStoped[i])	continue;
		dfm_info->snapUnLock();
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
	
	dfm_buff->setU32(mDfmInfo.size());
	map<u64, AosDfmInfoPtr>::iterator itr = mDfmInfo.begin();
	for(; itr != mDfmInfo.end(); itr++)
	{
		AosDfmInfoPtr dfm_info = itr->second;
		dfm_info->serializeTo(dfm_buff);
		dfm_info->stop();
	}
		
	mFile->put(0, dfm_buff->data(), dfm_buff->dataLen(), true); 
	
	OmnScreen << "AosDocFileMgrTester:: saveToFile end!" << endl;

	//mLock->lock();
	//mCondVar->signal();
	//mLock->unlock();
	
	return true;
}

/*
bool
AosDocFileMgrTester::readyKillSelf(const AosDfmInfoPtr &dfm_info)
{
	u32 val = (rand() % 2);
	if(val != 0)	return true;

	// first save to file.
	AosBuffPtr dfm_buff = OmnNew AosBuff(1000, 0 AosMemoryCheckerArgs);
	
	mStop = true;
	OmnSleep(2);	// wait all basicThrd stop.

	for(u32 i=0; i<eThreadNum; i++)
	{
		if(mThrdStoped[i])	continue;
		dfm_info->snapUnLock();
		OmnSleep(1);
		i--;
	}

	OmnScreen << "AosDocFileMgrTester:: saveToFile start!" << endl;

	dfm_buff->setU32(mDfmInfo.size());
	map<u64, AosDfmInfoPtr>::iterator itr = mDfmInfo.begin();
	for(; itr != mDfmInfo.end(); itr++)
	{
		AosDfmInfoPtr dfm_info = itr->second;
		dfm_info->serializeTo(dfm_buff);
	}
		
	mFile->put(0, dfm_buff->data(), dfm_buff->dataLen(), true); 
	
	OmnScreen << "AosDocFileMgrTester:: saveToFile end!" << endl;

	mLock->lock();
	mCondVar->signal();
	mLock->unlock();
	
	return true;
}
*/

