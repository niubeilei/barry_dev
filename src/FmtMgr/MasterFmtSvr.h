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
// Modification History:
// 06/17/2013	Created by Ketty
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_FmtMgr_MasterFmtSvr_h
#define AOS_FmtMgr_MasterFmtSvr_h

#include "API/AosApi.h"
#include "FmtMgr/Ptrs.h"
#include "ReplicMgr/Ptrs.h"
#include "Rundata/Ptrs.h"
#include "SEUtil/SeConfig.h"
#include "FmtMgr/FmtSvr.h"
#include "Thread/ThreadedObj.h"
#include "Util/Ptrs.h"

#include <vector>
#include <map>
using namespace std;

class AosMasterFmtSvr: public AosFmtSvr, public OmnThreadedObj
{
	OmnDefineRCObject;

	enum
	{
		eSendGlobalSize = 10000000, // 10M
		eCleanSleepTime = 5,		// 5 sec.
		
		eCleanThrdId,
	};
	
	struct BkpSvrInfo
	{
		int 	mSvrId;
		bool	mSvrDeathed;
		
		BkpSvrInfo()
		:
		mSvrId(-1),
		mSvrDeathed(false)
		{
		}
	};

private:
	OmnMutexPtr			mLock;
	bool				mStoped;

	OmnThreadPtr		mCleanThrd;
	AosFmtIdMgrPtr 		mIdMgr;

	vector<BkpSvrInfo>	mBkpSvrs;
	map<int, u64>		mFinishFmtIds;
	u64					mNextSendFmtId;

	OmnMutexPtr         mGlobalFmtLock;
	AosFmtPtr           mGlobalFmt;

	static __thread AosFmt *	stmFmt;
	
	AosFmtSvrPtr		mSwitchFrom;

public:
	AosMasterFmtSvr();
	~AosMasterFmtSvr();
	
	// ThreadedObj Interface
	virtual bool threadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread);
	virtual bool checkThread(OmnString &err, const int thrdLogicId) const{ return true;};
	virtual bool signal(const int threadLogicId){ return true; };

	virtual bool config(const AosXmlTagPtr &def);
	virtual bool start();
	virtual bool stop();
	
	void	setSwitchFrom(const AosFmtSvrPtr &from){ mSwitchFrom = from; };
	
	//bool 	reSwitch();

	virtual bool getNextSendFmtId(
					const AosRundataPtr &rdata,
					const u32 cube_grp_id,
					const int from_svr_id,
					u64 &next_fmt_id);

	virtual bool sendFmt(
					const AosRundataPtr &rdata,
					const AosFmtPtr	&fmt);

	virtual void setFmt(const AosFmtPtr &fmt){ stmFmt = fmt.getPtr(); }
	virtual AosFmtPtr getFmt() { return stmFmt; }

	virtual bool addToGlobalFmt(
				const AosRundataPtr &rdata,
				const u64 file_id,
				const u64 offset,
				const char *data,
				const int len,
				const bool flushflag);

	virtual bool addToGlobalFmt(
				const AosRundataPtr &rdata,
				const AosStorageFileInfo &file_info);

	virtual bool addDeleteFileToGlobalFmt(
				const AosRundataPtr &rdata, 
				const u64 &file_id);

	virtual bool addToGlobalFmt(
				const AosRundataPtr &rdata, 
				const AosFmtPtr &fmt);

	//==================== Bkp FmtSvr Func=========.
	virtual bool recvFmt(
				const AosRundataPtr &rdata,
				const u32 virtual_id,
				const int from_svrid,
				const AosBuffPtr &fmt_buff);

	virtual u64 getMaxRecvedFmtId();
	//==================== Bkp FmtSvr Func End=========.
	
private:
	bool 	switchFrom();
	bool 	init();
	bool 	initSvrIds();

	bool 	sendFmtToSvr(
				const AosRundataPtr &rdata,
				BkpSvrInfo &svr,
				const u64 fmt_id,
				const AosBuffPtr &fmt_buff);

	bool 	sendFmtPriv(
				const AosRundataPtr &rdata,
				const AosFmtPtr	&fmt);

	void 	sendGlobalFmtLocked(const AosRundataPtr &rdata);

	bool 	cleanThrdhreadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread);
	bool 	collectCleanFmtId(u64 &max_fmt_id);
	bool 	sendFinishedFmtId(const u64 max_fmt_id);

	bool 	saveToNormFile(
				const u64 fmt_id,
				const AosBuffPtr &fmt_buff);

};
#endif
