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
// 	Created: 05/09/2010 by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_SysMonitor_SysMonitor_h
#define AOS_SysMonitor_SysMonitor_h

#include "SystemMonitor/Ptrs.h"
#include "SingletonClass/SingletonObjId.h"
#include "SingletonClass/SingletonTplt.h"
#include "Thread/ThreadedObj.h"
#include "TransUtil/Ptrs.h"
//#include "TransUtil/AsyncReqTrans.h"
#include "Util/String.h"
#include "Util/Ptrs.h"
#include "Util/Buff.h"

#include <vector>
#include <bitset>
#include <map>
using namespace std;

OmnDefineSingletonClass(AosSysMonitorSingleton,
						AosSysMonitor,
						AosSysMonitorSelf,
						OmnSingletonObjId::eSysMonitor,
						"SysMonitor");

struct ServerMsg : public OmnRCObject
{
	OmnDefineRCObject;
	
	enum
	{
		eMaxSize = 100
	};

	int64_t							mStartPos;
	int64_t							mActivePos;
	OmnString						mErrorThrds;
	bool							mTempDeath;
	bool							mServerDeath;
	bitset<eMaxSize>				mStatus;
	bitset<eMaxSize>				mDeath;	
	map<OmnString, OmnString>		mErrorMaps;

	public:
	ServerMsg()
	:
	mStartPos(0),
	mActivePos(0),
	mErrorThrds(""),
	mTempDeath(false),
	mServerDeath(false)
	{
	}

	~ServerMsg(){}
};


class AosSysMonitor : public OmnThreadedObj,
					//public AosAsyncReqCaller
					virtual public OmnRCObject
{
	OmnDefineRCObject;

	enum
	{
		eDftSleepSec = 1000, //1s
		eDftDeathPercent = 60,
		eDftCreateLogTime = 60,
		eAskWaitSec = 5,
		eDftLogSec = 30,
		eDftLogSize = 9000
	};

public:
	OmnThreadPtr 					mThread;
	OmnMutexPtr						mLock;
	OmnString						mDftLogStr;
	OmnString						mLogStr;
	int								mSendTransNum;
	int								mServerNums;
	//map<bitset<eDftLogSec> >		mStatus;	
	//map<u32, OmnString>				mErrorThrds;
	//map<u32, int64_t>				mStartPos;
	//map<u32, int64_t>				mActivePos;
	u32								mStartTime;
	u32								mStartLogTime;
	vector<u32>						mSvrIds;
	map<int, ServerMsgPtr>			mServerMsgs;
	int								mSelfId;
	//map<u32, int> 					mDeath;
	int								mHeartBeatMSec;
	int								mDeathPercent;
	u32								mCreateLogTime;
	int								mCheckTime;
	//map<u32, bool>					mTempDeath;

private:
	AosSysMonitor();
	~AosSysMonitor();
public:
    // Singleton class interface
    static AosSysMonitor* 	getSelf();
    virtual bool      	start();
    virtual bool        stop();
    virtual bool		config(const AosXmlTagPtr &def);

	virtual bool threadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread);
	virtual bool signal(const int threadLogicId) {return true; }
//	virtual bool checkThread(OmnString &errmsg, const int tid) const {return true; }
	
	//void	callback(AosAsyncReqTransPtr &trans);
	void	askDeathCallback(
				const AosTransPtr &trans,
				const AosBuffPtr &resp,
				const bool svr_death);

	bool	composeLogStr(const AosBuffPtr &errorThrds);
	void	setServerDeath(const int serverid);
	bool	getStatus(const int serverid);

private:
	bool	composeLogStr(const OmnString &errorThrds);
	bool	addReq(const OmnString &errorThrds);
	bool	createLog();
	bool	addTrans(const AosTransPtr &trans);
	bool	checkServer();
	bool	askOtherServer(const int deathid);
	bool	tellOtherServer(const int deathid);
	bool	getErrorMsg(
				OmnString &errorMsg,
				map<OmnString, OmnString> &hasErrMsg);
};
#endif

