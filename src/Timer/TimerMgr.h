////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// Modification History:
// 2011/06/01	Created by Ken Lee
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_Timer_TimerMgr_h
#define AOS_Timer_TimerMgr_h

#include "Util/File.h"
#include "Thread/Mutex.h"
#include "Thread/CondVar.h"
#include "Thread/Thread.h"
#include "Thread/LockMonitorMgr.h"
#include "SingletonClass/SingletonObjId.h"
#include "SingletonClass/SingletonTplt.h"
#include "Timer/Timer.h"
#include "Timer/TimerObj.h"
#include "Timer/Ptrs.h"
#include "Util1/Timer.h"
#include "XmlUtil/XmlTag.h"

#include <queue>
#include <map>

OmnDefineSingletonClass(AosTimerMgrSingleton,
						AosTimerMgr,
						AosTimerMgrSelf,
						OmnSingletonObjId::eTimerMgr,
						"AosTimerMgr");

class AosTimerMgr : virtual public OmnRCObject,
					public OmnTimerObj,
					virtual public OmnThreadedObj
{
	OmnDefineRCObject;

	enum
	{
		eBodyBlockMgrOffset = 0,
		eBodyOffset = 5000,
		eLevelSize = 256,
		eMaxLevel = 4,
		eMaxBlocks = 2000,
		
		eBlockOffset = 150,
		eBlockFlagOffset = 0,
		eBlockIdxOffset = 1,
		eBlockTimerNumOffset = 5,
		eBlockNextBlockIdxOffset = 9,

		eBlockSize = 1500000
	};

	OmnFilePtr 				mFile;
	OmnString				mFileName;
	
	OmnMutexPtr 			mLock;
	AosLockMonitorPtr		mLockMonitor;
	OmnCondVarPtr  			mCondVar;
	OmnThreadPtr    		mThread;
	
	char *					mBlock;
	char *					mBlockMgrArray;
	int 					mLevelIdx[eMaxLevel];
	int 					mTimerid;
	
	bool					mIsValid;
	bool					mIsNewFile;
	bool					mIsInit;
	bool					mProcAgain;

	u32 					mSecond;
	queue<AosTimerPtr>		mTimerQueue;
	map<u64, AosRundataPtr>	mTimerMap;
	AosRundataPtr			mRundata;

public:
	AosTimerMgr();
	~AosTimerMgr();
	
	// Singleton class interface
	static AosTimerMgr*		getSelf();
	virtual bool			start();
	virtual bool			stop();
	virtual OmnString		getSysObjName() const {return "AosTimerMgr";}
	//virtual OmnRslt			config(const OmnXmlParserPtr &def){return true;}
	virtual OmnSingletonObjId::E	getSysObjId() const{ return OmnSingletonObjId::eTimerMgr; }
	virtual bool    config(const AosXmlTagPtr &config);

	// ThreadedObj Interface
	virtual bool    threadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread);
	virtual bool    signal(const int threadLogicId);
//	virtual bool    checkThread(OmnString &err, const int thrdLogicId) const;

	// TimerObj Interface
	virtual void 	timeout(const int timerid, const OmnString &timerName, void *parm);
	
	// Timer Interface
	bool			addTimer(
						const u64 &sdocid,
						const u32 &second, 
						u64 &timerid,
						const OmnString &udata,
						const AosRundataPtr &rdata);

	bool			addTimer(
						const u64 &sdocid,
						const u32 &second, 
						u64 &timerid,
						const OmnString &udata,
						const AosTimerObjPtr &caller,
						const AosRundataPtr &rdata);

	bool			cancelTimer(const u64 &timerid, const AosRundataPtr &rdata);

	bool			isValid() const { return mIsValid; }

private :
	bool			getFile();
	bool			initLevelIdx();
	bool			procTimeoutBlocks(const u32 &now, const int &level, const int &idx);

	bool			procTimeout();
	bool			procTimeoutOneBlock(const u32 now, const int idx);
	bool			createNewBlock(const int idx, const bool reflush);
	bool			procTimer(const AosTimerPtr &t);

	bool			refreshBlockMgrArray(const bool refresh);
	bool			freeBlock(const int &idx);
	
	bool			getValidBlockIdx(const int &blockIdx, int &newidx, int &num);
	bool			expandTimers(const u32 &now, const int &level);
	bool			expandOneBlock(const u32 &now, const int &level, const int &idx);
	
	bool			addTimer(
						const u64 &sdocid,
						const u32 &second,
						u64 &timerid,
						const OmnString &udata,
						const AosTimerType type,
						const AosRundataPtr &rdata);

	bool			getBucket(
						const u32 &second,
						int &level,
						int &idx,
						const AosRundataPtr &rdata);
	
	bool			saveTimer(
						const int &level, 
						const int &idx, 
						const AosTimerPtr &timer, 
						u64 &timerid);
	
	bool			saveTimer(
						const int &level, 
						const int &idx, 
						const char *str,
						u32 &offset);
	
	bool			checkControlEntry(
						AosBuff &buff,
						const int &blockIdx,
						int &numTimer,
						int &nextidx);
	
	bool			sanityCheck(const bool lock);
	bool			sanityCheck1(const int &i);
	bool			blockSanityCheck(AosBuff &buff, const int &num, const int &blockidx);
};
#endif
