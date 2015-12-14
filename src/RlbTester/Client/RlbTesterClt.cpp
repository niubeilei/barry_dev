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
#include "RlbTester/Client/RlbTesterClt.h"

#include "alarm_c/alarm.h"
#include "AppMgr/App.h"
#include "Debug/Debug.h"
#include "Porting/Sleep.h"
#include "RlbTester/Client/TesterCubeGrpInfo.h"
#include "SEInterfaces/ProcInfo.h"
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

#include "RlbTester/TesterTrans/RlbTesterCreateFileTrans.h"
#include "RlbTester/TesterTrans/RlbTesterModifyFileTrans.h"
#include "RlbTester/TesterTrans/RlbTesterDeleteFileTrans.h"
#include "RlbTester/TesterTrans/RlbTesterReadDataTrans.h"

int	AosRlbTesterClt::smMinSizes[eMaxLevel];
int AosRlbTesterClt::smMaxSizes[eMaxLevel];
int AosRlbTesterClt::smWeights[eMaxLevel];

AosLRUCache<u64, OmnMutexPtr> AosRlbTesterClt::smLockPool;
AosRlbTesterClt::LockGen AosRlbTesterClt::smLockGen;

AosRlbTesterClt::AosRlbTesterClt()
:
mLock(OmnNew OmnMutex()),
mShowLog(true)
{
	//memset(mSvrsStart, 0, sizeof(bool) * eSvrNum);
	
	smMinSizes[0] = eSizeLevel0; smMaxSizes[0] = eSizeLevel1; smWeights[0] = eLevel1Weight; 
	smMinSizes[1] = eSizeLevel1; smMaxSizes[1] = eSizeLevel2; smWeights[1] = eLevel2Weight; 
	smMinSizes[2] = eSizeLevel2; smMaxSizes[2] = eSizeLevel3; smWeights[2] = eLevel3Weight; 
	smMinSizes[3] = eSizeLevel3; smMaxSizes[3] = eSizeLevel4; smWeights[3] = eLevel4Weight; 
	
	mMaxFileSize = eMaxFileSize; 
	mMaxReadSize = 5000;
}


AosRlbTesterClt::~AosRlbTesterClt()
{
}


bool 
AosRlbTesterClt::start()
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

	// start thread
	OmnThreadedObjPtr thisPtr(this, false);
	for(u32 i=0; i<eThreadNum; i++)
	{
		OmnString str = "RlbTesterClt_BasicThrd";
		str << "_" << i;
		mBasicThrds[i] = OmnNew OmnThread(thisPtr, str,
				i, true, true, __FILE__, __LINE__);
		mBasicThrds[i]->start();
	}
	
	mMonitorThrd = OmnNew OmnThread(thisPtr, "RlbTesterClt_MonitorThrd",
			eThreadNum, true, true, __FILE__, __LINE__);
	mMonitorThrd->start();
	return true;
}


bool
AosRlbTesterClt::threadFunc(
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
		return monitorThrdFunc(state, thread);
	}
	return true;
}


bool
AosRlbTesterClt::basicThrdFunc(
		  OmnThrdStatus::E &state,
		  const OmnThreadPtr &thread)
{
	while(state == OmnThrdStatus::eActive)
	{
		basicTest();
	}
	
	u32 tid = thread->getLogicId();
	aos_assert_r(tid < eThreadNum, false);

	OmnScreen << "Thread:" << thread->getLogicId() 
		<< "; Existing!	" 
		<< "Total Num:" << eThreadNum 
		<< endl;

	thread->stop();
	return true;
}


bool
AosRlbTesterClt::monitorThrdFunc(
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

/*
bool
AosRlbTesterClt::killProc()
{
	int svr_id = -1;
	u32 proc_id = 0;
	bool rslt = grp_info->randGetActiveSvrInfo(svr_id, proc_id);
	aos_assert_r(rslt, false);
	if(svr_id == -1)	return true; 

	AosAppMsgPtr msg = OmnNew AosRlbTesterKillProcMsg(
		svr_id, AOSTAG_SVRPROXY_PID, proc_id);
	
	if(mShowLog)
	{
		OmnScreen << "RlbTesterClt; Monitor, killProc"
			<< "; svr_id:" << svr_id
			<< "; proc_id:" << proc_id 
			<< endl;
	}

	rslt = AosSendMsg(msg);
	aos_assert_r(rslt, false);
	grp_info->setSvrProcDeath(svr_id, proc_id);
	return true;
}


bool
AosRlbTesterClt::startProc()
{
	int svr_id = -1;
	u32 proc_id = 0;
	bool rslt = grp_info->randGetDeathSvrInfo(svr_id, proc_id);
	aos_assert_r(rslt, false);
	if(svr_id == -1)	return true; 

	AosAppMsgPtr msg = OmnNew AosRlbTesterStartProcMsg(
		svr_id, AOSTAG_SVRPROXY_PID, proc_id);
	
	if(mShowLog)
	{
		OmnScreen << "RlbTesterClt; Monitor, startProc"
			<< "; svr_id:" << svr_id
			<< "; proc_id:" << proc_id 
			<< endl;
	}
	
	rslt = AosSendMsg(msg);
	aos_assert_r(rslt, false);
	grp_info->setSvrProcActive(svr_id, proc_id);
	return true;
}
*/

AosTesterCubeGrpInfoPtr
AosRlbTesterClt::randGetCubeGrpInfo()
{
	aos_assert_r(mCubeGrpInfo.size(), 0);

	u32 idx = rand() % mCubeGrpInfo.size();
	map<u32, AosTesterCubeGrpInfoPtr>::iterator itr = mCubeGrpInfo.begin();
	while(idx--) itr++;	
	aos_assert_r(itr != mCubeGrpInfo.end(), 0);

	return itr->second;
}


bool 
AosRlbTesterClt::basicTest()
{
	// 1. random chose the app_id and pick the dfm
	AosRundataPtr rdata = OmnNew AosRundata(AosMemoryCheckerArgsBegin);
	rdata->setSiteid(1);

	// 2. random determain the opration
	bool rslt = false;
	switch (OmnRandom::percent(eCreateWeight, eModifyWeight, eDeleteWeight, eReadWeight))
	{
	case 0:
		rslt = createFile(rdata);
		break;
	
	case 1:
		rslt = modifyFile(rdata);
		break;

	case 2:
		rslt = deleteFile(rdata);
		break;
		
	case 3:
		readData(rdata);
		break;

	default:
		OmnAlarm << "Invalid percent!" << enderr;
		return false;
	}
	return rslt;
}


bool
AosRlbTesterClt::createFile(const AosRundataPtr &rdata)
{
	if(mFiles.size() >= eMaxFileNum)	return true;

	//u32 cube_id = rand() % AosGetNumCubes(); 
	u32 cube_id = 0; 
	AosTransPtr trans = OmnNew AosRlbTesterCreateFileTrans(
			cube_id, rdata->getSiteid(), eMaxFileSize);

	AosBuffPtr resp;
	bool svr_death;
	bool rslt = AosSendTrans(rdata, trans, resp, svr_death);
	aos_assert_r(rslt && resp, false);

	u64 file_id = resp->getU64(0);
	OmnString fname = resp->getOmnStr("");
	aos_assert_r(file_id && fname != "", false);

	rslt = addFileToLocal(file_id, fname);
	aos_assert_r(rslt, false);
	
	if(mShowLog)
	{
		OmnScreen << "RlbTesterClt; createFile"
			<< "; file_id:" << file_id
			<< "; fname:" << fname
			<< endl;
	}
	return true;
}


bool
AosRlbTesterClt::addFileToLocal(const u64 file_id, const OmnString fname)
{
	aos_assert_r(file_id && fname != "", false);

	OmnString full_fname = OmnApp::getAppBaseDir();
	full_fname << fname;
	OmnFilePtr file = OmnNew OmnFile(full_fname, OmnFile::eCreate AosMemoryCheckerArgs);
	aos_assert_r(file && file->isGood(), false);
	
	mLock->lock();
	mFiles.insert(make_pair(file_id, file));
	mLock->unlock();
	return true;
}


bool
AosRlbTesterClt::modifyFile(const AosRundataPtr &rdata)
{
	// It does the following:
	//  1. Randomly picks a file
	//  2. Randomly generates data whose size is randomly determined. 
	//  3. Randomly determine the offset
	if(mFiles.size() == 0)	return true;	

	OmnFilePtr file;
	u64 file_id;
	bool rslt = randGetFile(file, file_id);
	aos_assert_r(rslt && file_id > 0, false);

	// Randomly generate the data
	u32 pattern, repeat;
	AosBuffPtr data = randData(pattern, repeat);
	//u64 offset = rand() % (mMaxFileSize - data->dataLen());
	u64 offset = randGetOffset(file->getLength(), data->dataLen());

    OmnMutexPtr lock = smLockPool.get(file_id, smLockGen);
	lock->lock();	

	u32 cube_id = file_id >> 32;
	AosTransPtr trans = OmnNew AosRlbTesterModifyFileTrans(
			cube_id, file_id, offset, data); 

	AosBuffPtr resp;
	bool svr_death;
	rslt = AosSendTrans(rdata, trans, resp, svr_death);
	if (svr_death)
	{
		lock->unlock();
		return true;
	}

	aos_assert_rl(rslt && resp, lock, false);
	rslt = resp->getU8(0);
	aos_assert_rl(rslt, lock, false);
	
	file->put(offset, data->data(), data->dataLen(), true);
	lock->unlock();	
	
	if(mShowLog)
	{
		OmnScreen << "RlbTesterClt; modifyFile"
			<< "; file_id:" << file_id
			<< "; offset:" << offset
			<< "; pattern:" << pattern
			<< "; repeat:" << repeat
			<< endl;
	}

	return true;
}


bool
AosRlbTesterClt::deleteFile(const AosRundataPtr &rdata)
{
	if(mFiles.size() == 0)	return true;	
	
	OmnFilePtr file;
	u64 file_id;
	bool rslt = randGetFile(file, file_id);
	aos_assert_r(rslt && file_id > 0, false);

    OmnMutexPtr lock = smLockPool.get(file_id, smLockGen);
	lock->lock();
	
	u32 cube_id = file_id >> 32;
	AosTransPtr trans = OmnNew AosRlbTesterDeleteFileTrans(
			cube_id, file_id); 

	AosBuffPtr resp;
	bool svr_death;
	rslt = AosSendTrans(rdata, trans, resp, svr_death);
	aos_assert_rl(rslt && resp, lock, false);
	rslt = resp->getU8(0);
	aos_assert_rl(rslt, lock, false);

	rslt = deleteFileFromLocal(file_id);
	aos_assert_rl(rslt, lock, false);
	lock->unlock();

	if(mShowLog)
	{
		OmnScreen << "RlbTesterClt; deleteFile"
			<< "; file_id:" << file_id
			<< endl;
	}
	return true;
}


bool
AosRlbTesterClt::deleteFileFromLocal(const u64 file_id)
{
	aos_assert_r(file_id, false);

	mLock->lock();
	map<u64, OmnFilePtr>::iterator itr = mFiles.find(file_id);
	aos_assert_rl(itr != mFiles.end(), mLock, false);
	OmnFilePtr file = itr->second;
	OmnString full_fname = file->getFileName();

	mFiles.erase(itr);
	mLock->unlock();
	
	int rs = unlink(full_fname.data());
	aos_assert_r(rs >=0, false);

	return true;
}


bool
AosRlbTesterClt::readData(const AosRundataPtr &rdata)
{
	if(mFiles.size() == 0)	return true;	

	OmnFilePtr file;
	u64 file_id;
	bool rslt = randGetFile(file, file_id);
	aos_assert_r(rslt && file_id > 0, false);
    
	OmnMutexPtr lock = smLockPool.get(file_id, smLockGen);
	lock->lock();

	u64 file_len = file->getLength();
	if(file_len == 0)	
	{
		// Chen Ding: Need to verify the file stored in the system is empty.
		lock->unlock();
		return true;
	}
	u64 offset = rand() % file_len;
	u64 remain_size = file_len - offset;
	// u32 max_data_len = remain_size < 100 ? remain_size : 100; 
	u32 max_data_len = remain_size < mMaxReadSize ? remain_size : mMaxReadSize; 
	u32 data_len = rand() % max_data_len;
	aos_assert_r(offset + data_len <= file_len, false);

	if(mShowLog)
	{
		OmnScreen << "RlbTesterClt; ready read data"
			<< "; file_id:" << file_id
			<< "; offset:" << offset
			<< "; len:" << data_len
			<< endl;
	}
	
	AosBuffPtr l_data = OmnNew AosBuff(data_len, 0 AosMemoryCheckerArgs);
	int bytes_read = file->readToBuff(offset, data_len, l_data->data());
	aos_assert_rl(bytes_read == data_len, lock, false);

	// rand get svr_id.
	AosTransPtr trans = OmnNew AosRlbTesterReadDataTrans(
			file_id, offset, data_len);

	AosBuffPtr resp;
	bool svr_death;
	rslt = AosSendTrans(rdata, trans, resp, svr_death);
	aos_assert_rl(rslt, lock, false);
	if(svr_death)
	{
		lock->unlock();	
		return true;
	}

	rslt = resp->getU8(0);
	aos_assert_rl(rslt, lock, false);
	u32 r_data_len = resp->getU32(0);
	aos_assert_rl(r_data_len == data_len, lock, false);
	AosBuffPtr r_data = resp->getBuff(r_data_len, true AosMemoryCheckerArgs);

	int ret = memcmp(l_data->data(), r_data->data(), data_len);
	aos_assert_rl(ret == 0, lock, false);
	lock->unlock();
	
	return true;
}


bool
AosRlbTesterClt::randGetFile(OmnFilePtr &file, u64 &file_id)
{
	mLock->lock();
	u32 total_fnum = mFiles.size();	
	int idx = rand() % total_fnum;
	map<u64, OmnFilePtr>::iterator itr = mFiles.begin();
	while(idx--)	itr++;
	aos_assert_rl(itr != mFiles.end(), mLock, false);

	file_id = itr->first;
	file = itr->second;
	mLock->unlock();
	return true;
}


AosBuffPtr
AosRlbTesterClt::randData(u32 &pattern, u32 &repeat)
{
	pattern = rand();
	
	OmnString ss;
	ss << pattern;
	u32 pattern_len = ss.length();
	
	u32 need_size = OmnRandom::intByRange(smMinSizes, smMaxSizes, smWeights, eLevelNum);
	repeat = need_size / pattern_len;
	
	AosBuffPtr data = OmnNew AosBuff(need_size, 0 AosMemoryCheckerArgs);
	for(u32 i=0; i<repeat; i++)
	{
		data->setBuff(ss.data(), ss.length());
	}
	
	u64 data_size = pattern_len * repeat;
	aos_assert_r(data->dataLen() == data_size, 0);
	
	return data;
}


u64
AosRlbTesterClt::randGetOffset(const u64 crt_file_len, const u64 data_len)
{
	bool append = (rand() % 10 == 1);
	u64 offset;
	if(append)
	{
		offset = crt_file_len;
	}
	else
	{
		offset = rand() % (mMaxFileSize - data_len);
	}
	
	return offset;
}


