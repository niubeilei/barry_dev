////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// This type of IIL maintains a list of (string, docid) and is sorted
// based on the string value. 
//
// Modification History:
// 04/09/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_IILMerger_IILMerger_h
#define AOS_IILMerger_IILMerger_h

#include "API/AosApi.h"
#include "IILMerger/Ptrs.h"
#include "IILMerger/IILMergerBkt.h"
#include "IILMerger/IILMergerHandler.h"
#include "Rundata/Ptrs.h"
#include "Rundata/Rundata.h"
#include "Thread/Mutex.h"
#include "Thread/ThrdShellProc.h"
#include "Thread/Ptrs.h"
#include "Util/Ptrs.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/BuffArray.h"
#include <list>
#include <vector>

using namespace std;

class AosIILMerger : virtual public OmnRCObject
{
	OmnDefineRCObject;

private:
	list<AosIILMergerBktPtr> 	mBuckets;
	AosIILMergerHandlerPtr		mHandler;
	OmnMutexPtr					mLock;
	OmnMutexPtr					mTotalLock;
	OmnString 					mTableid;
	int 						mMaxBucket;
	int 						mTotalClient;
	int							mNumSyncs;			// Chen Ding, 06/23/2012
	int							mTotalFinished;		// Chen Ding, 06/23/2012
	u64							mFinishedEntries;	
	AosCompareFunPtr			mComp;
	u32							mMaxMemEntries;							 
	u32							mMaxEntries;							 
	u32							mLayer;
	u32							mFileId;
	int64_t						mAddTotal;
	int64_t						mAppendTotal;
	int64_t						mProcTotal;
	AosIILMerger				*mParentMerger;
	char*						mData;
	OmnFilePtr                  mFile;

	static OmnMutexPtr 					smLock; 
	static map<OmnString, AosIILMergerPtr>	smMergers;

public:
	class	BucketAppend : public OmnThrdShellProc
	{
		OmnDefineRCObject;

	public:
		AosIILMerger			*mMerger;
		AosIILMergerBkt			*mBkt;
		AosBuffPtr 				mBuff;
		char*					mCrt;
		int 					mNum;
		AosRundataPtr			mRundata;

	public:
		BucketAppend(
			AosIILMerger *merger,
			AosIILMergerBkt *bkt,
			const AosBuffPtr &buff,
			char*	crt,
			const int 	num,
			const AosRundataPtr &rdata)
		:
		OmnThrdShellProc("BucketAppend"),
		mMerger(merger),
		mBkt(bkt),
		mBuff(buff),
		mCrt(crt),
		mNum(num),
		mRundata(rdata->clone(AosMemoryCheckerArgsBegin))
		{
		}

		virtual	bool	run();
		virtual bool 	procFinished();
	};

public:
	class	MergeAllData : public OmnThrdShellProc
	{
		OmnDefineRCObject;

	public:
		AosIILMerger			*mMerger;
		AosRundataPtr			mRundata;

	public:
		MergeAllData(
			AosIILMerger *merger,
			const AosRundataPtr &rdata)
		:
		OmnThrdShellProc("MergeAllData"),
		mMerger(merger),
		mRundata(rdata->clone(AosMemoryCheckerArgsBegin))
		{
		}

		virtual	bool	run();
		virtual bool 	procFinished();
	};

public:
	class	BktMergeData : public OmnThrdShellProc
	{
		OmnDefineRCObject;

	public:
		vector<AosIILMergerBkt*> mBkts;
		AosRundataPtr			mRundata;

	public:
		BktMergeData(
			vector<AosIILMergerBkt*> bkts,
			const AosRundataPtr &rdata)
		:
		OmnThrdShellProc("BucketMergeData"),
		mBkts(bkts),
		mRundata(rdata->clone(AosMemoryCheckerArgsBegin))
		{
		}

		virtual	bool	run();
		virtual bool 	procFinished();
	};

public:
	class MergeData : public OmnThrdShellProc
	{
		OmnDefineRCObject;
	
	public:
		AosIILMerger		*mMerger;
		OmnString			mTableid;
		u64					mJobDocid;
		int					mJobServerId;
		OmnString			mTokenid;
		AosRundataPtr		mRundata;
	public:
		MergeData(
			AosIILMerger *merger,
			const OmnString &tableid,
			const u64 &jobdocid,
			const int &jobServerId,
			const OmnString &tokenid,
			const AosRundataPtr &rdata)
		:
		OmnThrdShellProc("MergeData"),
		mMerger(merger),
		mTableid(tableid),
		mJobDocid(jobdocid),
		mJobServerId(jobServerId),
		mTokenid(tokenid),
		mRundata(rdata->clone(AosMemoryCheckerArgsBegin))
		{
		}

		virtual bool run();
		virtual bool procFinished();
	};

public:
	class ToProc : public OmnThrdShellProc
	{
		OmnDefineRCObject;
	
	public:
		AosIILMerger		*mMerger;
		u32					mStart;
		OmnString			mTableid;
		int					mJobServerId;
		u64					mJobDocid;
		OmnString			mTokenid;
		AosRundataPtr		mRundata;
	public:
		ToProc(
			AosIILMerger *merger,
			const u32 start,
			const OmnString &tableid,
			const int &jobServerId,
			const u64 &jobdocid,
			const OmnString &tokenid,
			const AosRundataPtr &rdata)
		:
		OmnThrdShellProc("ToProc"),
		mMerger(merger),
		mStart(start),
		mTableid(tableid),
		mJobServerId(jobServerId),
		mJobDocid(jobdocid),
		mTokenid(tokenid),
		mRundata(rdata->clone(AosMemoryCheckerArgsBegin))
		{
		}

		virtual bool run();
		virtual bool procFinished();
	};

public:
	AosIILMerger(
		const AosIILMergerHandlerPtr &handler, 
		const int maxbkt,
		const u32 maxmementries, 
		const u32 maxentries, 
		const u32 layer,
		AosIILMerger *pmerger);
	~AosIILMerger();
	
	bool 		addDataSync(const AosBuffPtr &buff, const AosRundataPtr &rdata);
	bool 		addDataAsync(const AosBuffPtr &buff, const AosRundataPtr &rdata);

	AosIILMergerHandlerPtr getHandler(){return mHandler;}
	AosCompareFunPtr getComp(){return mComp;}
	u64 getNumEntries();
	OmnString createFileName()
	{
		// Chen Ding, 07/29/2012
		OmnString fname = AosGetBaseDirname();
		fname << "/mg_" << mTableid << "_" << mLayer << "_";
		return fname;
	}
	
	u32 		getLayer(){return mLayer;}
	int 		getMaxBucket(){return mMaxBucket;}
	bool 		isFinished() const;
	int 		compare(const char* lhs, const char* rhs);
	//bool 		mergeData(const u32 totalentries, const AosRundataPtr &rdata);
	bool 		mergeData(const AosBuffPtr &buff, const AosRundataPtr &rdata);
	bool 		mergeAllData(const AosRundataPtr &rdata);
	int 		getNumSyncs(){return mNumSyncs;}
	void		setTableid(const OmnString &tableid) {mTableid = tableid;}
	OmnString	getTableid() { return mTableid;}
	void		updateTotalClient() {++mTotalClient;}
	int 		getTotalClient() {return mTotalClient;}
	void		updateTotalFinished() {++mTotalFinished;}
	int 		getTotalFinished() {return mTotalFinished;}
	AosIILMerger*	getParentMerger(){return mParentMerger;}
	list<AosIILMergerBktPtr>::iterator findFreeBucket();

	void setNumSyncs(const int num_syncs)
	{
		if (num_syncs <= 0) return;
		if (num_syncs > mNumSyncs)
		{
			mNumSyncs = num_syncs;
		}
	}
	
	void updateAppendTotal(const int64_t total) 
	{
		mTotalLock->lock();
		mAppendTotal += total;
		mTotalLock->unlock();
		AosIILMerger* pmerger = getParentMerger();
		if (pmerger)
		{
			pmerger->updateAppendTotal(total);
		}
	}

	void updateProcTotal(const int64_t total) 
	{
		mTotalLock->lock();
		mProcTotal += total;
		mTotalLock->unlock();
		AosIILMerger* pmerger = getParentMerger();
		if (pmerger)
		{
			pmerger->updateProcTotal(total);
		}
	}

	void updateAddTotal(const int64_t total)
	{	
		mTotalLock->lock();
		mAddTotal += total;
		mTotalLock->unlock();
	}
	
	int64_t	getTotalEntries(){return mAddTotal;} 

	bool clear();
	bool serializeTo(const AosBuffPtr&, const AosRundataPtr&);
	bool serializeFrom(const AosBuffPtr&, const AosRundataPtr&);

	void addFinishedEntries(const u64 entries)
	{
		mFinishedEntries += entries;
	}
	
	bool mergeFinshed(
		const OmnString &tableid, 
		const u64 &jobdocid,
		const int &jobServerId,
		const OmnString &tokenid,
		const AosRundataPtr &rdata);

	bool procFinished();
	bool toProc(
		const u32 start,
		const OmnString &tableid, 
		const int &jobServerId,
		const u64 &jobdocid,
		const OmnString &tokenid,
		const AosRundataPtr &rdata);

private:
	bool addDataPriv(
			const AosBuffPtr &buff, 
			vector<OmnThrdShellProcPtr> &runner,
			const bool needmem,
			const AosRundataPtr &rdata);
	bool appendData(
			AosBuffArray &array, 
			vector<OmnThrdShellProcPtr> &runner,
			const AosRundataPtr &rdata);
	char* findBoundary(
			char *begin, 
			char *high,
			const int length);

	/*list<AosIILMergerBktPtr>::iterator findBucket(
			char **crt, 
			AosBuffArray &array, 
			vector<OmnThrdShellProcPtr> &runners,
			list<AosIILMergerBktPtr>::iterator &pos,
			const AosRundataPtr &rdata);
			*/
	bool splitRange(const char* start, const int length);
	bool sanitycheck(char *begin, int length);
	bool splitData(
			char *crt,
			AosBuffArray &array,
			const int num,
			const vector<AosIILMergerBktPtr> &buckets,
			vector<OmnThrdShellProcPtr> &runners,
			const AosRundataPtr &rdata);
	bool findBuckets(
			char *crt, 
			const int num,
			vector<AosIILMergerBktPtr> &buckets,
			list<AosIILMergerBktPtr>::iterator &pos);
};
#endif

