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
// Created: 04/17/2013 by Ketty 
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_TransServer_TransFileMgr_h
#define AOS_TransServer_TransFileMgr_h

#include "SvrProxyUtil/Ptrs.h"
#include "SvrProxyMgr/Ptrs.h"
#include "SEUtil/SeConfig.h"
#include "SEUtil/LogFile.h"
#include "Thread/ThreadedObj.h"
#include "Thread/Ptrs.h"
#include "TransServer/Ptrs.h"
#include "TransUtil/Ptrs.h"
#include "Util/TransId.h"
#include "Util/Ptrs.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "XmlUtil/Ptrs.h"

#include <set>
#include <map>
#include <queue>
using namespace std;

class AosTransFileMgr;
class AosReadRcd: public OmnRCObject 
{
	OmnDefineRCObject;
	
	friend class AosTransFileMgr;
private:	
	int 	mReadId;
	int		mCrtFileSeq;
	bool	mIsStaleFile;

	AosReadRcd(const int read_id)
	:
	mReadId(read_id),
	mCrtFileSeq(-1),
	mIsStaleFile(false)
	{
	};
};

class AosTransFileMgr: public OmnThreadedObj 
{
	OmnDefineRCObject;

	enum
	{
		eSaveThrdId = 1,
		eReadThrdId = 2,
		eSaveThrdSleep = 5 * 60,
		
		//eCleanTransId = 1,
		//eFinishTransId = 2,

		eMaxTransFileSize = 1000 * 1000 * 1000,	// 1G.
		
		eMaxTmpTids = 30,		// the same as TransClient eMaxWaitNum.
	};
	
	
	typedef set<AosTransId> set_t;
	typedef set<AosTransId>::iterator setitr_t;
	typedef map<u64, set_t>::iterator mapitr_t;
	
private:
	OmnMutexPtr			mLock;
	OmnMutexPtr			mReadLock;
	OmnCondVarPtr       mCondVar;
	AosTransServerPtr	mTransSvr;
	OmnThreadPtr		mCleanThrd;
	OmnThreadPtr		mReadThrd;
	AosTransFilePtr		mTransFile;
	OmnFilePtr			mMaxRecvFile;
	
	set_t				mFinishTids;
	AosLogFilePtr		mFinishLog;
	
	map<u64, set_t>		mTmpFinishTid;
		
	map<u32, AosReadRcdPtr>	mCrtReadRcd;
	queue<AosTransFileReqPtr> mReqs;

	int			mCrtCleanSeq;
	u32			mNextReadId;
	bool		mStarted;
	bool		mShowLog;

public:
	AosTransFileMgr(
		const AosTransServerPtr &trans_svr,
		const OmnString &dir_name,
		const bool show_log);
	~AosTransFileMgr();

	// ThreadedObj Interface
	virtual bool    threadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread);
	virtual bool    signal(const int threadLogicId){ return true;};
	virtual bool    checkThread(OmnString &err, const int thrdLogicId) const { return true; };

	void	start();
	void	stop();

	bool 	getMaxRecvTid(map<u64, u64> & max_recv_seq);

	bool 	saveToNormFile(const AosTransPtr &trans);
	bool 	saveToNewerFile(const AosTransPtr &trans);
	bool 	saveToNormFile(const AosBuffPtr &trans_buff);
	bool 	mergeNewerFiles();
	
	bool 	finishTrans(vector<AosTransId> &trans_ids);

	bool 	readUnfinishTrans(
				AosBuffPtr &trans_buff,
				int &read_id,
				bool &finish);
	
	bool	isFinish(AosTransId &trans_id);
	void	resetTransSvr(const AosTransServerPtr &trans_svr);

	bool 	normReqReadFinish(const int &read_id);

private:
	bool 	init(const OmnString &dir_name);
	bool 	initFinishTids();
	//bool 	initFinishTids(const AosBuffPtr &cont);
	bool 	saveFinishTids();

	bool 	cleanThrdFunc(
				OmnThrdStatus::E &state, 
				const OmnThreadPtr &thread);
	bool 	readFileThrdFunc(
				OmnThrdStatus::E &state, 
				const OmnThreadPtr &thread);

	bool 	addReadReq(const AosTransFileReqPtr &req);
	bool 	procReadReq(const AosTransFileReqPtr &req);
	AosReadRcdPtr getReadRcd(const AosTransFileReqPtr &req);
	bool 	getNextReadPos(const AosReadRcdPtr &read_rcd,
				int &new_seq,
				bool &new_is_stale);
	bool 	readTransPriv(
				const AosTransFileReqPtr &req,
				const int crt_seq,
				const bool is_stale);
	AosBuffPtr readBuffFromStaleFile(const int stale_seq);
	AosBuffPtr readBuffFromNormFile(const int file_seq, bool &finish);
	bool 	readUnfinishFromBuff(
				const AosTransFileReqPtr &req,
				const AosBuffPtr &cont,
				const bool save_to_stale);
	bool 	readNormFileFinish(
				const AosTransFileReqPtr &req,
				const int file_seq);
	bool 	reqReadFinish(const AosTransFileReqPtr &req);

	bool 	saveMaxRecvTid();
	bool 	readAllTransIds(const AosBuffPtr &cont,
				vector<AosTransId> &v_tids);
	
	//void 	cleanTransIds(vector<AosTransId> &v_tids);
	bool 	cleanFinishTids(vector<AosTransId> & clean_tids);
	bool 	cleanFinishLogs();

	bool 	isFinishPriv(const AosTransId &trans_id);
	bool 	addToTmpFinishTids(AosTransId &tid);

	bool 	cleanReqReadFinish(const AosTransFileReqPtr &req);

	bool 	addCleanReq(const AosTransFileReqPtr &req);
	bool 	addNormReadReq(const AosTransFileReqPtr &req);
	bool 	addReadReqLocked(const AosTransFileReqPtr &req);

	void 	printReadedTrans(const AosTransId &trans_id, const bool finish);
	void 	printCleanedTid(const AosTransId &tid);
	void 	printFinishedTid(const AosTransId &tid);

};
#endif
