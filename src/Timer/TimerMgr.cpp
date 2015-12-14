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
//  This class is used to manager the timer.
//
//  The file for saving the timers are divided into two parts.
//  The first (from 0 to eBodyOffset (default 100)) is used to save the information of the file.
//  The second (from eBodyOffset to end) is the timers contents.
//
//  The second is divided into eMaxLevel (default 4) parts.
//  each Level is divided into eLevelSize(default 256) parts.
//  So we total have eMaxLevel*eLevelSize blocks.
//  each block's size is eBlockSize (default 1500000)
//
//  Each block is also divided into two parts.
//  The first entry saves some information about this block, called Control 
//  entry. Its format is:
//  		error check flag 			1 byte
//  		total num of the timers		4 bytes (int) 
//  		block index					4 bytes (int)
//  		next						4 bytes (int)
//
//  The remaining entries are for all the timers, whose format is:
//  Each timer have 150 bytes.
//  The timer's structure as follows:
//  	checkflag  : bool , 1 bytes.
//  	offset 	   : u32  , 4 bytes.
//  	isCanceled : bool , 1 bytes.
//  	Spend Time : u32  , 4 bytes.
//  	Start Time : u32  , 4 bytes.
//  	End Time   : u32  , 4 bytes.
//  	timerId    : u64  , 8 bytes.
//  	sdocid	   : u64  , 8 bytes.
//  	udata	   : string ......
//	
// Modification History:
// 2011/06/01	Created by Ken Lee
// 2011/06/15	Modified by Ken Lee
////////////////////////////////////////////////////////////////////////////
#include "Timer/TimerMgr.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Debug/Debug.h"
#include "Porting/TimeOfDay.h"
#include "Thread/LockMonitor.h"
#include "Util/OmnNew.h"
#include "Util/Buff.h"
#include "XmlUtil/XmlTag.h"
#include "SingletonClass/SingletonImpl.cpp"
#include <map>

OmnSingletonImpl(AosTimerMgrSingleton,
		         AosTimerMgr,
				 AosTimerMgrSelf,
				 "AosTimerMgr");


AosTimerMgr::AosTimerMgr():
mLock(OmnNew OmnMutex()),
mCondVar(OmnNew OmnCondVar()),
mBlock(0),
mTimerid(0),
mIsValid(false),
mIsNewFile(false),
mIsInit(false),
mProcAgain(false)
{
	mLockMonitor = OmnNew AosLockMonitor(mLock);
	mLock->mMonitor = mLockMonitor;
	AosLockMonitorMgr::getSelf()->addLock(mLockMonitor);

	mBlock = OmnNew char[eBlockSize];
	mBlockMgrArray = OmnNew char[eMaxBlocks];
	memset(mBlockMgrArray, '0', eMaxBlocks);
	mTimerMap.clear();

	OmnThreadedObjPtr thisptr(this, false);
	mThread = OmnNew OmnThread(thisptr, "TimerMgr", 0, true, true, __FILE__, __LINE__);
	mThread->start();
	
	mRundata = OmnApp::getRundata();
	aos_assert(mRundata);
	mRundata = mRundata->clone(AosMemoryCheckerArgsBegin);
}


AosTimerMgr::~AosTimerMgr()
{
	OmnDelete []mBlock;
	OmnDelete []mBlockMgrArray;
}


bool
AosTimerMgr::start()
{
	return true;
}


bool
AosTimerMgr::stop()
{
	return true;
}

//felicia, 2013/06/19
/*	
bool
AosTimerMgr::signal(const int threadLogicId)
{
	return true;
}


bool
AosTimerMgr::checkThread(OmnString &err, const int thrdLogicId) const
{
	return true;
}
*/

bool
AosTimerMgr::signal(const int threadLogicId)
{
	mCondVar->signal();	
	return true;
}

bool
AosTimerMgr::config(const AosXmlTagPtr &config)
{
	aos_assert_r(config, false);

	AosXmlTagPtr timercfg = config->getFirstChild(AOSCONFIG_TIMERMGR);
	if (!timercfg) return false;
	
	OmnString dirname = timercfg->getAttrStr(AOSCONFIG_TIMER_DIRNAME, ".");
	aos_assert_r(dirname != "", false);

	OmnString fname = timercfg->getAttrStr(AOSCONFIG_TIMER_FNAME);
	aos_assert_r(fname != "", false);

	// Ketty 2013/05/16
	//mFileName = "";
	mFileName = OmnApp::getAppBaseDir();
	mFileName << dirname << "/" << fname;
	
	bool rslt = getFile();
	aos_assert_r(rslt, false);
	
	AOSLMTR_ENTER(mLockMonitor);
	AOSLMTR_LOCK(mLockMonitor);
	mFile->lock();
	rslt = initLevelIdx();
	mFile->unlock();
	AOSLMTR_UNLOCK(mLockMonitor);
	AOSLMTR_FINISH(mLockMonitor);

	aos_assert_r(rslt, false);
	mSecond = OmnGetSecond();
	
	u32 time = OmnGetUsec();
	int timedelt = 1000000 - time;
	OmnTimerObjPtr thisptr(this, false);
	mTimerid = OmnTimer::getSelf()->startTimer("AosTimerMgr", 0, timedelt, thisptr, 0);
	mIsValid = true;
	return true;
}
	

bool
AosTimerMgr::getFile()
{
	mFile = OmnNew OmnFile(mFileName, OmnFile::eReadWrite AosMemoryCheckerArgs);
	if(mFile && mFile->isGood()) return true;

	mFile = OmnNew OmnFile(mFileName, OmnFile::eCreate AosMemoryCheckerArgs);
	if(!mFile->isGood())
	{
		OmnAlarm << "********** Failed to create the Timer file : " << mFileName << enderr;
		return false;
	}

	mIsNewFile = true;
	bool rslt;
	mFile->lock();
	for(int i=0; i<eLevelSize*eMaxLevel; i++)
	{
		rslt = createNewBlock(i, false);
		if(!rslt)
		{
			mFile->unlock();
			return false;
		}
		mBlockMgrArray[i] = '1';
	}
	rslt = refreshBlockMgrArray(true);
	if(!rslt)
	{
		mFile->unlock();
		return false;
	}
	sanityCheck(false);
	mFile->unlock();
	return true;
}

	
bool
AosTimerMgr::initLevelIdx()
{
	// call this function should be locked
	mIsInit = true;
	u32 now = OmnGetSecond();
	u32 time = now;
	int i = eMaxLevel - 1;
	while(i >= 0)
	{
		time = now << (eMaxLevel - i - 1) * 8;
		mLevelIdx[i] = time >> (eMaxLevel - 1) * 8;
		i--;
	}
	
	int r = mFile->readToBuff(eBodyBlockMgrOffset, eMaxBlocks, mBlockMgrArray);
	aos_assert_r(r != -1, false);

	if(mIsNewFile)
	{
		mIsInit = false;
		return true;
	}
	
	u32 fileModifyTime = OmnFile::getLastModifyTime(mFile->getFileName());
	if(fileModifyTime == 0)
	{
		OmnAlarm << "fileLastModifyTime error" << enderr;
		return false;
	}

	int lastIdx[eMaxLevel];
	i = eMaxLevel - 1;
	while(!(time = fileModifyTime >> (i * 8)) && i >=0)
	{
		lastIdx[i] = time;
		i--;
	}

	bool rslt;
	for(i=eMaxLevel-1; i>=0; i--)
	{
		if(mLevelIdx[i] > lastIdx[i])
		{
			mLevelIdx[i]--;

			rslt = procTimeoutBlocks(now, i, mLevelIdx[i] - 1);
			aos_assert_r(rslt, false);
			
			rslt = expandTimers(now, i);
			aos_assert_r(rslt, false);
			
			if(i > 0) lastIdx[i - 1] = 0;
		}
	}
	mCondVar->signal();
	mIsInit = false;
	return true;
}


void 
AosTimerMgr::timeout(const int timerId, const OmnString &timerName, void *parm)
{
	aos_assert(timerId == mTimerid);
	
	mProcAgain = true;
	while(mProcAgain)
	{
		procTimeout();
	}
	
	u32 time = OmnGetUsec();
	int timedelt = 1000000 - time;
	OmnTimerObjPtr thisptr(this, false);
//OmnScreen << "timedelt:" << timedelt << endl;
	mTimerid = OmnTimer::getSelf()->startTimer("AosTimerMgr", 0, timedelt, thisptr, 0);
}


bool    
AosTimerMgr::threadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread)
{
	AOSLMTR_ENTER(mLockMonitor);
	while (state == OmnThrdStatus::eActive)
	{
		AOSLMTR_LOCK(mLockMonitor);
		if (mTimerQueue.empty())
		{
			AOSLMTR_WAIT(mLockMonitor);
			mCondVar->wait(mLock);
			mThreadStatus = true;
			AOSLMTR_UNLOCK(mLockMonitor);
			continue;
		}
		
		AosTimerPtr t = mTimerQueue.front();
		mTimerQueue.pop();
		AOSLMTR_UNLOCK(mLockMonitor);

		procTimer(t);
	}
	AOSLMTR_FINISH(mLockMonitor);
	return true;
}

	
bool 
AosTimerMgr::procTimer(const AosTimerPtr &t)
{
	// A timer expired. This function retrieves the smart doc and
	// the user information. It then runs the smart doc. 
	AosRundataPtr rdata = mRundata;
	rdata->reset();
	switch (t->getType())
	{
	case eXmlTimer: 
		 return t->procXmlTimer(rdata);

	case eMemoryTimer: 
		 {
			u64 timerid = t->getTimerId();
			map<u64, AosRundataPtr>::iterator itr = mTimerMap.find(timerid);
			if(itr != mTimerMap.end())
			{
				rdata = itr->second;
				mTimerMap.erase(itr);
			}

			return t->procMemoryTimer(rdata);
		 }
		 break;

	default :
		 OmnAlarm << "Unrecognized timer type: " << t->getType() << enderr;
		 return false;
	}
	return true;
}


bool 
AosTimerMgr::procTimeoutBlocks(const u32 &now, const int &level, const int &idx)
{
	int blockIdx = level * eLevelSize + idx;
	bool rslt;
	for(int i = 0; i <= blockIdx; i++)
	{
		rslt = procTimeoutOneBlock(now, i);
		aos_assert_r(rslt, false);
	}
	return true;
}


bool
AosTimerMgr::addTimer(
		const u64 &sdocid,
		const u32 &second,
		u64 &timerid,
		const OmnString &udata,
		const AosRundataPtr &rdata)
{
OmnScreen << "second : " << second << endl;
	aos_assert_r(mIsValid, false);
	return addTimer(sdocid, second, timerid, udata, eXmlTimer, rdata);
}


bool
AosTimerMgr::addTimer(
		const u64 &sdocid,
		const u32 &second, 
		u64 &timerid,
		const OmnString &udata,
		const AosTimerObjPtr &caller,
		const AosRundataPtr &rdata)
{
	aos_assert_r(mIsValid, false);
	
	bool rslt = addTimer(sdocid, second, timerid, udata, eMemoryTimer, rdata);
	aos_assert_r(rslt, false);

	AOSLMTR_ENTER(mLockMonitor);
	AOSLMTR_LOCK(mLockMonitor);
	rdata->setTimerCaller(caller);
	mTimerMap.insert(make_pair(timerid, rdata));
	AOSLMTR_UNLOCK(mLockMonitor);
	AOSLMTR_FINISH(mLockMonitor);
	return true;
}


bool
AosTimerMgr::addTimer(
		const u64 &sdocid,
		const u32 &second,
		u64 &timerid,
		const OmnString &udata,
		const AosTimerType type,
		const AosRundataPtr &rdata)
{
	AOSLMTR_ENTER(mLockMonitor);
	AOSLMTR_LOCK(mLockMonitor);
	mFile->lock();

//	bool flag = false;
	u32 now = OmnGetSecond();
	u32 sec = second;
	
	int level, idx;
	if(now == mSecond + 1)
	{
		sec++;
//		flag = true;
	}
	bool rslt = getBucket(sec, level, idx, rdata);
	if(!rslt)
	{
		OmnAlarm << "failed to get the bucket" << enderr;
		mFile->unlock();
		AOSLMTR_UNLOCK(mLockMonitor);
		AOSLMTR_FINISH(mLockMonitor);
		return false;
	}
	AosTimerPtr timer = OmnNew AosTimer(sdocid, now, second, type, rdata, udata);
	rslt = saveTimer(level, idx, timer, timerid);
//OmnScreen << "add  timer,timerid:" << timerid << ",second:" << second << ",mStartTime:" << now << endl;
//OmnScreen << "add timer, timerid:" << timer.getTimerId() <<", now:"<<now<<",endtime:"<<timer.getEndTime()<<",level:"<<level<<",idx:"<<idx<<endl;

//	if(flag)
//	{
//OmnScreen << "add timer, second+1, timerid:" << timer.getTimerId() <<", now:"<<now<<",endtime:"<<timer.getEndTime()<<",level:"<<level<<",idx:"<<idx<<endl;
//	}
	mFile->unlock();
	AOSLMTR_UNLOCK(mLockMonitor);
	AOSLMTR_FINISH(mLockMonitor);
	return rslt;
}


bool
AosTimerMgr::getBucket(
		const u32 &second,
		int &level,
		int &idx,
		const AosRundataPtr &rdata)
{
	if (second == 0)
	{
		rdata->setError() << "Timer value is 0";
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}
		
	int addIdx[eMaxLevel];
	level = 0;
	while(level < eMaxLevel)
	{
		addIdx[level] = (second << (eMaxLevel - 1 - level) * 8) >> (eMaxLevel - 1) * 8;
		level++;
	}
	
	level = 0;
	while(level < eMaxLevel)
	{
		idx = mLevelIdx[level] + addIdx[level] - 1;
		if(idx >= eLevelSize)
		{
			if(level+1 > eMaxLevel) return false;
			addIdx[level+1]++;		
		}
		level++;
	}

	level = eMaxLevel - 1; 
	while(level >= 0)
	{
		if(addIdx[level] > 0)
		{
			idx = mLevelIdx[level] + addIdx[level] - 1;
			return true;
		}
		level--;
	}
	return false;
}


bool
AosTimerMgr::saveTimer(
		const int &level,
		const int &idx,
		const AosTimerPtr &timer,
		u64 &timerid)
{
	int blockidx, numTimer;
	bool rslt = getValidBlockIdx(level * eLevelSize + idx, blockidx, numTimer);
	if(!rslt)
	{
		OmnAlarm << "failed to get the next valid blockidx" << enderr;
		return false;
	}

	u32 blockOffset = eBodyOffset + blockidx * eBlockSize;
	u32 timerOffset = blockOffset + eBlockOffset + numTimer * eTimerSize;
	rslt = timer->setTimerId(timerOffset, timerid);
	aos_assert_r(rslt, false);

	AosBuff buff(eTimerSize, 0 AosMemoryCheckerArgs);
	rslt = timer->setBuff(buff);
	aos_assert_r(rslt, false);

sanityCheck(false);
	rslt = mFile->put(timerOffset, buff.data(), eTimerSize, false);
	aos_assert_r(rslt, false);
	
	rslt = mFile->setInt(blockOffset + eBlockTimerNumOffset, ++numTimer, true);
	aos_assert_r(rslt, false);

sanityCheck(false);
	return true;
}


bool
AosTimerMgr::saveTimer(
		const int &level,
		const int &idx,
		const char * str,
		u32 &offset)
{
	int blockidx, numTimer;
	bool rslt = getValidBlockIdx(level * eLevelSize + idx, blockidx, numTimer);
	if(!rslt)
	{
		OmnAlarm << "failed to get the next valid blockidx" << enderr;
		return false;
	}
	
	u32 blockOffset = eBodyOffset + eBlockSize * blockidx;
	offset = blockOffset + eBlockOffset + numTimer * eTimerSize;

sanityCheck(false);
	rslt = mFile->put(offset, str, eTimerSize, false);
	aos_assert_r(rslt, false);
	
	rslt = mFile->setInt(blockOffset + eBlockTimerNumOffset, ++numTimer, false);
sanityCheck(false);
	return rslt;
}


bool
AosTimerMgr::cancelTimer(const u64 &timerid, const AosRundataPtr &rdata)
{
	aos_assert_r(mIsValid, false);
	
	u32 offset = timerid >> 32;
	u32 endTime = (u32) timerid;
	
	u32 second = OmnGetSecond();
	if(endTime < second)
	{
		OmnAlarm << "cancel error, timeout, timerid:" << timerid << ", second:" << second << ", endTime:" << endTime << enderr;
		return false;
	}
	
	AOSLMTR_ENTER(mLockMonitor);
	AOSLMTR_LOCK(mLockMonitor);
	mFile->lock();
	
	int guard = eMaxLevel;
	u32 timerOffset;
	while(offset && guard--)
	{
		timerOffset = offset;
		offset = mFile->readBinaryU32(offset + eTimerExpandOffset, 0);
	}

	u64 id  = mFile->readBinaryU64(timerOffset + eTimerIdOffset, 0);
	if (id != timerid)
	{
		mFile->unlock();
		AOSLMTR_UNLOCK(mLockMonitor);
		AOSLMTR_FINISH(mLockMonitor);
		OmnAlarm << "the timerid is not match" << enderr;
		return false;
	}
	
	bool rslt = mFile->setChar(timerOffset + eTimerCancelOffset, AOS_TIMER_CANCELED_FLAG, true);
	if(!rslt)
	{
		mFile->unlock();
		AOSLMTR_UNLOCK(mLockMonitor);
		AOSLMTR_FINISH(mLockMonitor);
		return false;
	}

	map<u64, AosRundataPtr>::iterator itr = mTimerMap.find(timerid);
	if(itr != mTimerMap.end())
	{
		mTimerMap.erase(itr);
	}
	
	mFile->unlock();	
	AOSLMTR_UNLOCK(mLockMonitor);
	AOSLMTR_FINISH(mLockMonitor);
//OmnScreen << "cancel success, timerid:" << timerid <<endl;
	return true;
}


bool    
AosTimerMgr::procTimeout()
{
	mProcAgain = false;
	//this funtion called by one thread
sanityCheck(true);
	AOSLMTR_ENTER(mLockMonitor);
	AOSLMTR_LOCK(mLockMonitor);
	mFile->lock();
	
	bool rslt;
	u32 now = OmnGetSecond();
	mSecond++;
	if(mSecond > now)
	{
		return false;
	}
	else if(mSecond < now)
	{
		mProcAgain = true;
		//OmnAlarm << "time error, mSecond:" << mSecond << ", now:" << now << enderr;
	}
	
	if (mLevelIdx[0] >= eLevelSize)
	{
		rslt = expandTimers(now, 1);
		if(!rslt)
		{
			mFile->unlock();
			AOSLMTR_UNLOCK(mLockMonitor);
			AOSLMTR_FINISH(mLockMonitor);
			return false;
		}
		mLevelIdx[0] = 0;
	}
	int blockIdx = mLevelIdx[0];
	mLevelIdx[0]++;
	procTimeoutOneBlock(now, blockIdx);
	
	mCondVar->signal();
	mFile->unlock();
	AOSLMTR_UNLOCK(mLockMonitor);
	AOSLMTR_FINISH(mLockMonitor);
	return true;
}


bool    
AosTimerMgr::procTimeoutOneBlock(const u32 now, const int idx)
{
//OmnScreen << "procTimeoutOneBlock, idx:" << idx << ", now:"<<now<<endl;
	u32 blockOffset = eBodyOffset + eBlockSize * idx;
	int r = mFile->readToBuff(blockOffset, eBlockSize, mBlock);
	aos_assert_r(r != -1, false);

	int numTimer = 0, nextidx;
	AosBuff buff(mBlock, eBlockSize, eBlockSize, false AosMemoryCheckerArgs);
	bool rslt = checkControlEntry(buff, idx, numTimer, nextidx);
	aos_assert_r(rslt, false);

	u32 endtime;
	for (int i = 0; i< numTimer; i++)
	{
		AosTimerPtr timer = OmnNew AosTimer(&mBlock[eBlockOffset + eTimerSize * i]);
		endtime = timer->getEndTime();
		if(endtime != now && !mIsInit)
		{
			OmnAlarm << "proc error, endtime:" << endtime << ",now:" << now << ",timerid:" << timer->getTimerId() << enderr;
		}
//OmnScreen << "push into queue, timerid:" << timer.getTimerId() << ", endtime:" << timer.getEndTime() << endl;
		mTimerQueue.push(timer);
	}
	
	if(nextidx == -1)
	{
		rslt = mFile->setInt(blockOffset + eBlockTimerNumOffset, 0, true);
		aos_assert_r(rslt, false);
	}
	else
	{
		rslt = mFile->setInt(blockOffset + eBlockTimerNumOffset, 0, false);
		aos_assert_r(rslt, false);
		
		rslt = mFile->setInt(blockOffset + eBlockNextBlockIdxOffset, -1, false);
		aos_assert_r(rslt, false);
		
		rslt = procTimeoutOneBlock(now, nextidx);
		aos_assert_r(rslt, false);
		
		mBlockMgrArray[nextidx] = '0';
		rslt = refreshBlockMgrArray(true);
		aos_assert_r(rslt, false);
//OmnScreen << "free timeout block , idx : " << nextidx << endl;
	}
	return true;
}


bool    
AosTimerMgr::expandTimers(const u32 &now, const int &level)
{
//OmnScreen << "i'm expanding the timer block, level:"<< level<<",idx:"<<mLevelIdx[level] << endl;
	int blockIdx = level * eLevelSize + mLevelIdx[level];
	bool rslt = freeBlock(blockIdx - 1);
	aos_assert_r(rslt, false);
	
	if(mLevelIdx[level] >= eLevelSize)
	{
		rslt = expandTimers(now, level + 1);
		aos_assert_r(rslt, false);
		mLevelIdx[level] = 0;
	}
	blockIdx = level * eLevelSize + mLevelIdx[level];
	rslt = expandOneBlock(now, level, blockIdx);
	aos_assert_r(rslt, false);
	
	mLevelIdx[level]++;
	mFile->flushFileContents();
	return true;
}


bool
AosTimerMgr::expandOneBlock(const u32 &now, const int &level, const int &blockidx)
{
	u32 offset = eBodyOffset + blockidx * eBlockSize;  
	int r = mFile->readToBuff(offset, eBlockSize, mBlock);
	aos_assert_r(r != -1, false);

	int numTimer = 0, nextidx;
	AosBuff buff(mBlock, eBlockSize, eBlockSize, false AosMemoryCheckerArgs);
	bool rslt = checkControlEntry(buff, blockidx, numTimer, nextidx);
	aos_assert_r(rslt, false);

	//add Timer to low level 
	multimap<u32, pair<char*, u32> > timers;
	timers.clear();
	u32 timerOffset, endtime, second;
	char * c;
	for (int i = 0; i < numTimer; i++)
	{
		timerOffset = eBlockOffset + eTimerSize * i;
		buff.setCrtIdx(timerOffset + eTimerEndTimeOffset);
		endtime = buff.getU32(0);
		if(endtime < now)
		{
			OmnAlarm << "endtime error, endtime:" << endtime << ", now:" << now << enderr;
			second = 0;
		}
		else
		{
			second = endtime - now;
		}
		c = &mBlock[timerOffset];
		timers.insert(make_pair(second, make_pair(c, timerOffset)));
	}

	multimap<u32, pair<char*, u32> >::iterator itr;
	u32 expandOffset;
	int idx = 0, i;
	for (itr = timers.begin(); itr != timers.end(); ++itr)
	{
		second = itr->first;
		idx = second;
		i = level - 1;
		while(i>0)
		{
			idx /= eLevelSize;
			i--;
		}
//OmnScreen << "expand timer, level:" << level << ", second : " << second << ", idx:" << idx << endl;

		rslt = saveTimer(level - 1, idx, itr->second.first, expandOffset);
		aos_assert_r(rslt, false);
		
		u32 oldOffset = itr->second.second;
		buff.setCrtIdx(oldOffset + eTimerExpandOffset);
		rslt = buff.setU32(expandOffset);
		aos_assert_r(rslt, false);
	}
	
	buff.setCrtIdx(eBlockTimerNumOffset);
	rslt = buff.setInt(0);
	aos_assert_r(rslt, false);

	rslt = mFile->put(offset, mBlock, eBlockSize, false);
	aos_assert_r(rslt, false);

	if(nextidx != -1)
	{
//OmnScreen << "expand next block ,idx:" << nextidx << endl;
		rslt = expandOneBlock(now, level, nextidx);
		aos_assert_r(rslt, false);
	}
	return true;
}


bool
AosTimerMgr::checkControlEntry(AosBuff &buff, const int &blockIdx, int &num, int &nextidx)
{
	// It checks the control entry:
	// 		flag			1 byte		AOS_TIMER_FLAG
	// 		num				4 bytes		[0, eMaxEntriesPerBlock]
	// 		index			4 bytes		[0, eMaxBlocks]
	// 		next			4 bytes		-1, or [1024, eMaxBlocks]
	buff.setCrtIdx(eBlockFlagOffset);
	char flag = buff.getChar(AOS_TIMER_FLAG);
	if(flag != AOS_TIMER_FLAG)
	{
		OmnAlarm << "flag error, flag = " << flag << enderr;
		return false;
	}
	
	buff.setCrtIdx(eBlockIdxOffset);
	int idx = buff.getInt(-1);
	if(idx != blockIdx)
	{
		OmnAlarm << "idx error, idx = " << idx << enderr;
		return false;
	}

	buff.setCrtIdx(eBlockTimerNumOffset);
	num = buff.getInt(0);
	if(num < 0)
	{
		OmnAlarm << "num error, num = " << num << enderr;
		return false;
	}

	buff.setCrtIdx(eBlockNextBlockIdxOffset);
	nextidx = buff.getInt(-1);
	if(nextidx == -1 || (nextidx >= 1024 && nextidx < eMaxBlocks))
	{
		return true;
	}
	else
	{
		OmnAlarm << "nextidx error, nextidx = " << nextidx << enderr;
		return false;
	}
	return true;
}


bool 
AosTimerMgr::getValidBlockIdx(const int &blockIdx, int &newidx, int &num)
{
	newidx = blockIdx;
	aos_assert_r(newidx != -1, false);
	
	u32 blockOffset = eBodyOffset + eBlockSize * newidx;
	char flag = mFile->readChar(blockOffset + eBlockFlagOffset, AOS_TIMER_FLAG);
	aos_assert_r(flag == AOS_TIMER_FLAG, false);

	num = mFile->readBinaryInt(blockOffset + eBlockTimerNumOffset, 0);
	aos_assert_r(num >= 0, false);

	if(eBlockOffset + num * eTimerSize < eBlockSize - eTimerSize) return true;

	bool rslt;
	int nextidx = mFile->readBinaryInt(blockOffset + eBlockNextBlockIdxOffset, -1);
	if(nextidx == -1)
	{
		for(int i = eMaxLevel * eLevelSize; i < eMaxBlocks; i++)
		{
			if(mBlockMgrArray[i] == '0') 
			{
				nextidx = i;
				rslt = createNewBlock(nextidx, false);
				aos_assert_r(rslt, false);
				break;
			}
		}
		rslt = mFile->setInt(blockOffset + eBlockNextBlockIdxOffset, nextidx, false);
		aos_assert_r(rslt, false);

		mBlockMgrArray[nextidx] = '1';
		rslt = refreshBlockMgrArray(true);
		aos_assert_r(rslt, false);
			
		num = 0;
		newidx = nextidx;
		return true;
	}
	else if (nextidx >= eLevelSize * eMaxLevel && nextidx < eMaxBlocks)
	{
		return getValidBlockIdx(nextidx, newidx, num);
	}
	return false;
}


bool
AosTimerMgr::createNewBlock(const int idx, const bool refresh)
{
//OmnScreen << "create new block idx:" << idx << endl;
	u32 blockOffset = eBodyOffset + eBlockSize * idx;
	bool rslt = mFile->setChar(blockOffset + eBlockFlagOffset, AOS_TIMER_FLAG, false);
	if(!rslt)
	{
		OmnAlarm << "init block flag error, idx:" << idx << enderr;
		return false;
	}
		
	rslt = mFile->setInt(blockOffset + eBlockIdxOffset, idx, false);
	if(!rslt)
	{
		OmnAlarm << "init block idx error, idx:" << idx << enderr;
		return false;
	}
			
	rslt = mFile->setInt(blockOffset + eBlockTimerNumOffset, 0, false);
	if(!rslt)
	{
		OmnAlarm << "init block timer num error, idx:" << idx << enderr;
		return false;
	}

	rslt = mFile->setInt(blockOffset + eBlockNextBlockIdxOffset, -1, refresh);
	if(!rslt)
	{
		OmnAlarm << "init block nextblockidx error, idx:" << idx << enderr;
		return false;
	}
	sanityCheck1(idx);
	return true;
}


bool
AosTimerMgr::refreshBlockMgrArray(const bool refresh)
{
	u64 blockMgrOffset = eBodyBlockMgrOffset;
	return mFile->setStr(blockMgrOffset, mBlockMgrArray, eMaxBlocks, refresh);
}


bool
AosTimerMgr::sanityCheck1(const int &i)
{
	return true;

	// Chen Ding, 2015/05/25
	// char *buffadd = OmnNew char[eBlockSize];
	// int num , nextidx;
	// int blockOffset = eBodyOffset + eBlockSize * i;
	// mFile->readToBuff(blockOffset, eBlockSize, buffadd);
	// AosBuff buff(buffadd, eBlockSize, eBlockSize AosMemoryCheckerArgs);
	// return checkControlEntry(buff, i, num, nextidx);
}


bool
AosTimerMgr::sanityCheck(const bool lock)
{
return true;	
	if (lock)
	{
		AOSLMTR_ENTER(mLockMonitor);
		AOSLMTR_LOCK(mLockMonitor);
	}
	char *buffadd = OmnNew char[eBlockSize];
	int blockOffset, nextidx, num;
	bool rslt;
	for(int i = 0; i < eMaxBlocks; i++)
	{
		if(mBlockMgrArray[i] == '0') continue;
		blockOffset = eBodyOffset + eBlockSize * i;
		mFile->readToBuff(blockOffset, eBlockSize, buffadd);
		AosBuff buff(buffadd, eBlockSize, eBlockSize, false AosMemoryCheckerArgs);
		rslt = checkControlEntry(buff, i, num, nextidx);
		if(!rslt || (nextidx != -1 && mBlockMgrArray[nextidx] == '0'))
		{
			if (lock)
			{
				AOSLMTR_UNLOCK(mLockMonitor);
				AOSLMTR_FINISH(mLockMonitor);
			}
			OmnDelete [] buffadd;
			return false;
		}
		rslt = blockSanityCheck(buff, num, i);
		if(!rslt)
		{
			if (lock)
			{
				AOSLMTR_UNLOCK(mLockMonitor);
				AOSLMTR_FINISH(mLockMonitor);
			}
			OmnDelete [] buffadd;
			return false;
		}
	}
	if (lock)
	{
		AOSLMTR_UNLOCK(mLockMonitor);
		AOSLMTR_FINISH(mLockMonitor);
	}
	OmnDelete [] buffadd;
	return true;
}


bool
AosTimerMgr::blockSanityCheck(
		AosBuff &buff, 
		const int &num,
		const int &blockidx)
{
	// This function checks the correctness of the block 'buff'. 
	// A block is correct if and only if all timer entries are correct.
	// A timer entry is correct if and only if:
	//  	checkflag  : bool , 1 bytes.		[
	//  	offset 	   : u32  , 4 bytes.		[0, xxx]
	//  	isCanceled : bool , 1 bytes.		y, n
	//  	Spend Time : u32  , 4 bytes.		duration, 
	//  	Start Time : u32  , 4 bytes.
	//  	End Time   : u32  , 4 bytes.
	//  	timerId    : u64  , 8 bytes.
	//  	sdocid	   : u64  , 8 bytes.
	//  	udata	   : string ......
	u32 now = OmnGetSecond();
	u32 endtime, second, offset;
	int level = blockidx / eLevelSize;
	int idx = blockidx % eLevelSize;

	u32 s = idx - mLevelIdx[level];
	int i = level;
	while(--i && i >= 0)
	{
		s *= eLevelSize;
	}

	for(int i=0; i<num; i++)
	{
		offset = eBlockOffset + eTimerSize * num + eTimerEndTimeOffset;
		buff.setCrtIdx(offset);
		endtime = buff.getU32(0);
		second = endtime - now;
		if(second < s)
		{
			OmnAlarm << "second is error , endtime:" << endtime << ", now:" << now << ", second:" << second << ", blockidx:" << blockidx << ", No.:" << i << ", num:" << num << enderr; 
			return false;
		}
	}
	return true;
}


bool
AosTimerMgr::freeBlock(const int &idx)
{
	u32 offset = eBodyOffset + eBlockSize * idx + eBlockNextBlockIdxOffset;
	int nextidx = mFile->readBinaryInt(offset, -1);
	while(nextidx != -1 && nextidx >= eMaxLevel * eLevelSize && nextidx < eMaxBlocks)
	{
		mBlockMgrArray[nextidx] = '0';
		offset = eBodyOffset + eBlockSize * nextidx + eBlockNextBlockIdxOffset;
		nextidx = mFile->readBinaryInt(offset, -1);
	}
	return refreshBlockMgrArray(true);
}


