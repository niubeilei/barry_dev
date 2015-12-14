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
#ifndef AOS_FmtMgr_BkpFmtSvr_h
#define AOS_FmtMgr_BkpFmgSvr_h

#include "API/AosApi.h"
#include "FmtMgr/Ptrs.h"
#include "ReplicMgr/Ptrs.h"
#include "Rundata/Ptrs.h"
#include "SEUtil/SeConfig.h"
#include "FmtMgr/FmtSvr.h"
#include "Util/Ptrs.h"

#include <vector>
#include <map>
using namespace std;

class AosBkpFmtSvr: public AosFmtSvr
{
	OmnDefineRCObject;
	
	struct SyncedSvrInfo
	{
		int 	mSvrId;
		bool	mSvrDeathed;
		int		mCrtReadFileSeq;	
		bool	mReadFinish;
		
		SyncedSvrInfo()
		:
		mSvrId(-1),
		mSvrDeathed(false),
		mCrtReadFileSeq(-1),
		mReadFinish(false)
		{
		}
	};


private:
	OmnMutexPtr		mLock;
	int             mCrtMaster;
	u64				mMaxFmtIdInFile;
	AosFmtProcerPtr mFmtProc;
	
	vector<SyncedSvrInfo>	mSyncSvrs;
	bool			mSyncFinish;
	u64				mRealSyncEnd;
	u64				mCrtSyncEnd;
	
	u64				mCrtRecvFmtId;

public:
	AosBkpFmtSvr(const int crt_master);
	~AosBkpFmtSvr();

	virtual bool config(const AosXmlTagPtr &def);
	virtual bool stop();
	
	virtual u64  getMaxRecvedFmtId();
	virtual bool recvFinishedFmtId(const u64 max_fmt_id);
	virtual bool recvFmt(
				const AosRundataPtr &rdata,
				const u32 cube_grp_id,
				const int from_svrid,
				const AosBuffPtr &fmt_buff);
	
	bool 	syncFmt();
	bool 	setNewMaster(const int new_master);
	
private:
	bool 	initSvrIds();
	
	bool 	addProcSeqLocked();
	u32 	askMasterNextSendFmtId();
	bool 	syncFmtPriv();
	bool 	getNextExpectSyncEnd(u64 &expect_sync_end);
	u64 	getMergeBegFmtId();
	int 	getNextSyncSvr(const int crt_sid);
	bool 	syncFmtFromSvr(
				SyncedSvrInfo &svr,
				const u64 sync_beg,
				const u64 expect_sync_end,
				bool &succ);
	bool 	sendReadFmtTrans(
				const int svr_id,
				const int file_seq,
				const u64 sync_beg,
				const u64 expect_sync_end,
				bool &svr_death,
				AosBuffPtr &resp);
	bool 	recvFmtBySync(
				const AosBuffPtr &cont,
				const u64 expect_sync_end,
				bool &sync_succ);

	bool 	recvNewerFmt(
				const AosRundataPtr &rdata,
				const u64 fmt_id,
				const AosBuffPtr &fmt_buff);
	bool 	recvNormFmt(
				const AosRundataPtr &rdata,
				const u64 fmt_id,
				const AosBuffPtr &fmt_buff);
	bool 	saveToNormFile(
				const AosRundataPtr &rdata, 
				const AosBuffPtr &fmt_buff);

};
#endif
