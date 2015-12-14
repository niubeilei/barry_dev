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
// 06/07/2011	Created by Linda Lin 
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_DocLock_DocLock_h
#define AOS_DocLock_DocLock_h

#include "Util/String.h"
#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Debug/Debug.h"
#include "SEUtil/Ptrs.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/SPtr.h"
#include "DocLock/Ptrs.h"
#include "Thread/Mutex.h"
#include "Thread/ThreadedObj.h"
#include "Thread/Thread.h"
#include "Thread/Ptrs.h"
#include "Rundata/Rundata.h"
#include "SingletonClass/SingletonObjId.h"
#include "SingletonClass/SingletonTplt.h"

OmnDefineSingletonClass(AosDocLockSingleton,
		                AosDocLock,
						AosDocLockSelf,
						OmnSingletonObjId::eDocLock,
						"DocLock");

class AosDocLock;
struct AosStLock: public OmnRCObject
{
private:
	OmnDefineRCObject;

public:
	u64 lockid;
	u64 req_userid;
	u64 timer;
};

struct AosStLockReq: public OmnRCObject 
{
private:
	OmnDefineRCObject;

public:
	enum REQ
	{
		eInvalid,
		eReadUnLockReq,
		eWriteUnLockReq,
		eMax
	};

public:
	u64 docid;
	u64 lockid;
	REQ reqtype;
	AosRundataPtr rdata;
};

struct AosHashKey
{
	u64 docid;
	u32 siteid;
	AosHashKey(u64 did, u32 sid):docid(did),siteid(sid){};

	bool operator == (const AosHashKey &rhs) const
	{
		return (docid==rhs.docid) && (siteid==rhs.siteid);
	}
};

struct AosStHash
{
	size_t operator() (const AosHashKey &p) const
	{
		return (p.docid + (p.siteid << 16)) / 24551;
	}
};

struct compare_St
{
	bool operator()(const AosHashKey &p1, const AosHashKey &p2) const
	{

		return p1 == p2;
	}
};

#include "Util/HashUtil.h"
#include <vector>
#include <queue>
using namespace std;

typedef pair< vector<AosStLockPtr>*, vector<AosStLockPtr>* >		AosDocLockPair;
//typedef hash_map<const u64, AosDocLockPair, AosU64Hash1, u64_cmp>  	AosU642LockPair_t;
typedef hash_map<AosHashKey, AosDocLockPair, AosStHash, compare_St>  	AosU642LockPair_t;

//typedef hash_map<const u64, AosDocLockPair, AosU64Hash1, u64_cmp>::iterator  AosU642LockPairItr_t;
typedef hash_map<AosHashKey, AosDocLockPair, AosStHash, compare_St>::iterator  AosU642LockPairItr_t;

class AosDocLock : virtual public OmnThreadedObj, public AosTimerObj
{
	OmnDefineRCObject;	

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
		OmnMutexPtr			mLock;
		AosU642LockPair_t	mDocLock;
		queue<vector<AosStLockPtr>*> mCtnrs;
		OmnThreadPtr    	mThread;
		queue<AosStLockReqPtr> mReq;
		int 				mNumReqs;
		OmnCondVarPtr		mCondVar;

public:
	AosDocLock();
	~AosDocLock() ;

	//Singleton class interface
	static AosDocLock*   getSelf();
	virtual bool        start();
	virtual bool        stop();                                    
	virtual bool	    config(const AosXmlTagPtr  &config);

	//ThreadedObj Interface
	virtual bool    threadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread);
	virtual bool    signal(const int threadLogicId);
//	virtual bool    checkThread(OmnString &err, const int thrdLogicId) const;

	static E toEnum(const OmnString &name)
	{
		// rl --- readlock; ru --- readulock; wl --- writelock; wu --- writeunlock
		if (name == "rl" || name == "readlock") return eReadLock;
		if (name == "ru" || name == "readunlock") return eReadUnLock;
		if (name == "wl" || name == "writelock") return eWriteLock;
		if (name == "wu" || name == "writeunlock")return eWriteUnLock;
		return eInvalid;
	}

public:
	// This is the LocMgr Interface
	bool	readLock(const AosRundataPtr &rdata, const u64 &docid, const u32 &lock_timer, const u64 &transid);
	bool	readUnLock(const AosRundataPtr &rdata, const u64 &docid, const u64 &lockid);
	bool	checkRead(const AosRundataPtr &rdata, const u64 &docid);
	bool	writeUnLock(const AosRundataPtr &rdata,	const u64 &docid, const u64 &lockid);
	bool	writeLock(const AosRundataPtr &rdata, const u64 &docid, const u32 &lock_timer, const u64 &transid);
	bool	checkModify(const AosRundataPtr &rdata, const u64 &docid);
	bool 	timeout(const u64 &timerid, const OmnString &udata,  const AosRundataPtr &rdata);

private:
	//bool 	timeout(const u64 &timerid, const AosRundataPtr &rdata);
	bool 	findReadLockPriv(
				const AosRundataPtr &rdata,
				const u64 &docid, 
				const u64 &lockid, 
				bool &found,
				AosU642LockPairItr_t &itermap);
	bool  	readLockPriv(
				const AosRundataPtr &rdata, 
				const u64 &docid, 
				const u32 &lock_timer, 
				const u64 &transid,
				u64 &lockid);
	bool 	readUnLockPriv(
				const AosRundataPtr &rdata, 
				const u64 &docid, 
				const u64 &lockid,
				const AosU642LockPairItr_t &itercheck);
	bool 	findWriteLockPriv(
				const AosRundataPtr &rdata,
				const u64 &docid, 
				const u64 &lockid, 
				bool &found, 
				AosU642LockPairItr_t &itermap);
	bool	writeLockPriv(
				const AosRundataPtr &rdata,
				const u64 &docid, 
				const u32 &lock_timer, 
				const u64 &transid,
				u64 &lockid);
	bool 	writeUnLockPriv(
				const AosRundataPtr &rdata,
				const u64 &docid, 
				const u64 &lockid,
				const AosU642LockPairItr_t &itercheck);
	bool 	canWriteLockPriv(const u64 &docid, const AosRundataPtr &rdata);
	bool	canReadLockPriv(const u64 &docid, const AosRundataPtr &rdata);
	vector<AosStLockPtr>*	getVectorPriv();
	void	returnVectorPriv(vector<AosStLockPtr>* v);
	bool	checkModifyPriv(const AosRundataPtr &rdata, const u64 &docid);
	bool	checkReadPriv(const AosRundataPtr &rdata, const u64 &docid);
	bool	sanityCheck(
				const AosRundataPtr &rdata, 
				AosU642LockPairItr_t itercheck,
				const u64 &docid);
	bool	sanityCheck2(
				const AosRundataPtr &rdata,
				AosU642LockPairItr_t itercheck,
				const u64 &docid);
	bool	sanityCheck3();
	bool	sanityCheck4(const AosRundataPtr &rdata, const u64 &docid);
	void	addReqToQueue(const AosStLockReqPtr &req);
	bool	createTimerPriv(
			const AosStLockReq::REQ &type,
			const u32 lock_timer,
			const u64 &docid,
			const u64 &lockid,
			const AosRundataPtr &rdata);
	AosStLockReqPtr getReqFromQueue();

protected:
		
};
#endif

