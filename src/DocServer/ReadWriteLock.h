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
// 2014/10/10 	Created by Felicia Peng
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_DocServer_ReadWriteLock_h
#define AOS_DocServer_ReadWriteLock_h

#include "Util/String.h"
#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Debug/Debug.h"
#include "SEUtil/Ptrs.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/SPtr.h"
#include "DocServer/Ptrs.h"
#include "Thread/Mutex.h"
#include "Thread/ThreadedObj.h"
#include "Thread/Thread.h"
#include "Thread/Ptrs.h"
#include "TransBasic/Ptrs.h"
#include "TransBasic/Trans.h"
#include "Rundata/Rundata.h"
#include "SingletonClass/SingletonObjId.h"
#include "SingletonClass/SingletonTplt.h"


class AosReadWriteLock;
struct AosRWLock: public OmnRCObject
{
private:
	OmnDefineRCObject;

public:
	u64 req_userid;
	u32 start_time;
	int wait_timer;
	int hold_timer;
};

struct AosRWLockReq: public OmnRCObject 
{
private:
	OmnDefineRCObject;

public:
	enum REQ
	{
		eInvalid,
		eReadLock,
		eWriteLock,
		eMax
	};

public:
	u64 docid;
	u64 req_userid;
	u32	start_time;
	int wait_timer;
	int hold_timer;
	AosTransPtr trans;
	REQ reqtype;
	AosRundataPtr rdata;
};


#include "Util/HashUtil.h"
#include <vector>
#include <queue>
using namespace std;

typedef pair< vector<AosRWLockPtr>*, AosRWLockPtr>							AosReadWriteLockPair;

typedef hash_map<u64, AosReadWriteLockPair, u64_hash, u64_cmp>				AosDocid2LockPair_t;
typedef hash_map<u64, AosReadWriteLockPair, u64_hash, u64_cmp>::iterator   	AosDocid2LockPairItr_t;

typedef hash_map<u64, deque<AosRWLockReqPtr> *, u64_hash, u64_cmp>				AosDocid2ReqPair_t;
typedef hash_map<u64, deque<AosRWLockReqPtr> *, u64_hash, u64_cmp>::iterator   	AosDocid2ReqPairItr_t;

class AosReadWriteLock : virtual public OmnThreadedObj
{
	OmnDefineRCObject;	

	struct Info
	{
		OmnString		type;
		OmnString		lock_type;
		OmnString		start_time;
		OmnString		wait_timer;
		OmnString		hold_timer;
		OmnString		user_id;
		AosTransPtr		trans;
	};

public:
	enum E
	{
		eInvalid,
		eReadLock,
		eReadUnLock,
		eWriteLock,
		eWriteUnLock,
		eMax
	};

private:
		OmnMutexPtr						mLock;
		AosDocid2LockPair_t				mReadWriteLock;
		queue<vector<AosRWLockPtr>*> 	mOmnNewCtnrs;
		OmnThreadPtr    				mThread;
		AosDocid2ReqPair_t				mReq;
		int 							mNumReqs;
		OmnCondVarPtr					mCondVar;

public:
	AosReadWriteLock();
	~AosReadWriteLock() ;
	
	//ThreadedObj Interface
	virtual bool    threadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread);
	virtual bool    signal(const int threadLogicId);


	bool			readLock(
						const u64 &docid,
						const u32 &userid,
						const int waittimer,
						const int holdtimer,
						const AosTransPtr &trans,
						bool &wait,
						AosRundata *rdata);
	
	bool			writeLock(
						const u64 &docid,
						const u32 &userid,
						const int waittimer,
						const int holdtimer,
						const AosTransPtr &trans,
						bool &wait,
						AosRundata *rdata);

	bool			unLockRead(
						const u64 &docid,
						const u64 &userid,
						AosRundata* rdata);

	bool			unLockWrite(
						const u64 &docid,
						const u64 &userid,
						const AosXmlTagPtr &newdoc,
						const AosTransId &trans_id,
						//bool &lock_expired,
						AosRundata* rdata);

private:
	vector<Info>		getWaiterExpires();

	bool				getReadExpireInfo(
							vector<Info> &expired_reqs,
							const AosRWLockReqPtr &req);

	bool				getWriteExpireInfo(
							vector<Info> &expired_reqs,
							const AosRWLockReqPtr &req);

	AosRWLockReqPtr		getReqFromQueue(
							const u64 &docid,
							AosDocid2ReqPairItr_t &req_itr);
	
	bool				canReadLock(
							const u64 &docid, 
							const u64 &userid,
							bool &canlock,
							AosRundata *rdata);

	bool				canWriteLock(
							const u64 &docid, 
							const u64 &userid,
							bool &canlock,
							AosRundata *rdata);

	bool				addReadLock(
							const u64 &docid,
							const u32 &userid,
							const int waittimer,
							const int holdtimer,
							AosRundata *rdata);

	bool				addReadWaitLock(
							const u64 &docid,
							const u32 &userid,
							const int waittimer,
							const int holdtimer,
							const AosTransPtr &trans,
							AosRundata *rdata);

	bool				addWriteLock(
							const u64 &docid,
							const u32 &userid,
							const int waittimer,
							const int holdtimer,
							AosRundata *rdata);


	bool				addWriteWaitLock(
							const u64 &docid,
							const u32 &userid,
							const int waittimer,
							const int holdtimer,
							const AosTransPtr &trans,
							AosRundata *rdata);

	bool				findReadLock(
							const u64 &docid,
							const u64 &userid,
							bool &found,
							AosDocid2LockPairItr_t &itr,
							AosRundata* rdata);

	bool				findWriteLock(
							const u64 &docid,
							const u64 &userid,
							bool &found,
							AosDocid2LockPairItr_t &itr,
							AosRundata* rdata);

	bool				procWriteWaitReq(
							const AosRWLockReqPtr &req, 
							const u64 &docid,
							//const AosDocid2LockPairItr_t &itr,
							AosRundata* rdata);

	bool				procReadWaitReq(
							const AosRWLockReqPtr &req, 
							const u64 &docid,
							//const AosDocid2LockPairItr_t &itr,
							const AosDocid2ReqPairItr_t &req_itr,
							AosRundata* rdata);

	vector<AosRWLockPtr>*	getVector();
	void				returnVector(vector<AosRWLockPtr>* v);
	bool				checkExpiredLocks();
};
#endif

