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
// 05/15/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_DataBlob_DataBlob_h
#define Aos_DataBlob_DataBlob_h

#include "DataBlob/Ptrs.h"
#include "DataBlob/DataBlobType.h"
#include "DataBlob/DataBlobFullHandler.h"
#include "DataRecord/DataRecord.h"
#include "Rundata/Ptrs.h"
#include "SEInterfaces/Ptrs.h"
#include "SEInterfaces/DataBlobObj.h"
#include "Thread/ThreadedObj.h"
#include "Thread/ThrdShellProc.h"
#include "Thread/Ptrs.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/BuffArray.h"
#include "Util/Ptrs.h"
#include "XmlUtil/Ptrs.h"
#include <vector>
#include <queue>
using namespace std;


class AosDataBlob : virtual public AosDataBlobObj, virtual public OmnThreadedObj
{
	OmnDefineRCObject;        

private:
	enum
	{
		eMaxThrds = 5,
		eMaxRcdSize = 100,
		eDftPageSize = 1000,
		eMaxMemSize = 1000000000
	};

public:
	enum E
	{
		eInvalid,
		
		eFull,
		eStart,
		eFinish,
		eAppend,

		eMax
	};

public:
	class ActionRunner : public OmnThrdShellProc
	{
		OmnDefineRCObject;

	public:
		AosDataBlobObjPtr		mBlob;
		int64_t					mStartIdx;
		int64_t					mNumEntries;
		vector<AosActionObjPtr> mActions;
		AosDataRecordObjPtr		mDataRecord;
		AosRundataPtr			mRundata;

	public:
		~ActionRunner();
		ActionRunner(
			const AosDataBlobObjPtr &blob,
			const int64_t start_idx,
			const int64_t num_entries, 
			const vector<AosActionObjPtr> &actions,
			const AosDataRecordObjPtr &record,
			const AosRundataPtr &rdata);

		virtual bool        run();
		virtual bool		procFinished();
	};

	struct AosRunActionsReq
	{
		bool							run_in_thrds_shell;
		AosBuffArrayPtr					data;
		vector<AosActionObjPtr>			actions_before_sort;
		vector<AosActionObjPtr>			actions_after_sort;
		AosRundataPtr					rdata;

		AosRunActionsReq(
				const bool				run_in_thrds_shell,
				const AosBuffArrayPtr	&data,
				const vector<AosActionObjPtr> &actions_before_sort,
				const vector<AosActionObjPtr> &actions_after_sort,
				const AosRundataPtr	&rdata)
			:
			run_in_thrds_shell(run_in_thrds_shell),
			data(data),
			actions_before_sort(actions_before_sort),
			actions_after_sort(actions_after_sort),
			rdata(rdata)
		{
		}

		~AosRunActionsReq()
		{
		}
	};

protected:
	AosDataBlobType::E			mType;
	bool						mIsSorted;
	bool						mIsStable;
	OmnMutexPtr         		mLock;
	OmnCondVarPtr       		mCondVar;
	vector<AosActionObjPtr>		mFullActionsBeforeSort;
	vector<AosActionObjPtr>		mFullActionsAfterSort;
	vector<AosActionObjPtr>		mFinishActionsBeforeSort;
	vector<AosActionObjPtr>		mFinishActionsAfterSort;
	vector<AosActionObjPtr>		mStartActions;
	vector<AosActionObjPtr>		mAppendActions;
	vector<AosActionObjPtr>		mFilteredActions;
	vector<AosActionObjPtr>		mInvalidActions;
	bool						mRunFullActionsInBackground;
	bool						mNeedToSortOnFull;
	AosDataBlobFullHandlerPtr	mFullHandler;
	u64 						mMaxNumRecords;
	u64							mMaxMemSize;
	u64							mPageSize;
	AosDataRecordObjPtr			mDataRecord;
	OmnThreadPtr				mThread;
	queue<AosRunActionsReq>		mPendingReqs;
	bool						mRunInThrdShell;
	AosXmlTagPtr				mConfig;
	AosBuffDataPtr			mMetaData;

public:
	AosDataBlob(
			const OmnString &name, 
			const AosDataBlobType::E type);
	AosDataBlob(const AosDataBlob &rhs);
	~AosDataBlob();
	
	// OmnThreadedObj Interface
	virtual bool    threadFunc(OmnThrdStatus::E &state, 
			                    const OmnThreadPtr &thread);
	virtual bool    signal(const int threadLogicId);
	virtual bool    checkThread(OmnString &err, const int thrdLogicId) const;
	
	virtual bool 	sort() = 0;
	virtual int64_t size() const = 0;
	virtual u64		getMemSize() const = 0;
	virtual bool	clearData() = 0;
	virtual bool	setData(const char *record, const u64 &len) = 0;
	virtual int  	getRecordLen() const = 0;
	virtual	int 	getRecordLen(const int idx) const = 0;
	virtual u64     getRewriteLoopIdx() const = 0;
	virtual AosDataBlobObjPtr clone() = 0;
	virtual AosDataBlobObjPtr clone(const AosXmlTagPtr &def, const AosRundataPtr &rdata) = 0;
	virtual AosDataRecordObjPtr cloneDataRecord() const;
	
	// This interface for all subclass.	
	virtual	void	resetRecordLoop() = 0;
	virtual	void	resetRangeLoop() = 0;
	virtual bool 	resetRewriteLoop() = 0;
	virtual bool 	setRewriteLoopIdx(const u64 idx) = 0;
	virtual bool 	rewriteRecordByIdx(
						const u64 idx,
						const AosDataRecordObjPtr &record,
						const AosRundataPtr &rdata) = 0;
	virtual bool 	rewriteNextRecord(
						const AosDataRecordObjPtr &record,
						const bool append_if_overflow, 
						const AosRundataPtr &rdata) = 0;
	
	virtual bool nextRecord(const AosDataRecordObjPtr &record) = 0;
	virtual bool getRecord(const int64_t &idx, const AosDataRecordObjPtr &record) const = 0;
	virtual const char *getMemory(
						const int64_t &start_idx, 
						const int64_t &end_idx,
						int64_t &length, 
						const AosRundataPtr &rdata) const = 0;
	
	// This interface for AosBlobRecordStr
	virtual bool appendRecord(
					const AosDataRecordObjPtr &record,
					const AosRundataPtr &rdata) = 0;

	virtual bool appendEntry(
					const AosValueRslt &value,
					const AosRundataPtr &rdata) = 0;

	virtual bool start(const AosRundataPtr &rdata) = 0;
	virtual bool finish(const AosRundataPtr &rdata) = 0;

	virtual AosDataBlobObjPtr createDataBlob(
					const AosXmlTagPtr &def, 
					const AosRundataPtr &rdata);
	virtual AosDataBlobObjPtr createDataBlob(
					const AosDataRecordObjPtr &record,
					const AosRundataPtr &rdata);
	virtual AosDataBlobObjPtr createDataBlob(
					const OmnString &sep,
					const AosRundataPtr &rdata);

	bool 	isSorted() const {return mIsSorted;}
	void	setSorted(const bool isSorted){mIsSorted = isSorted;}	
	bool 	isStable() const {return mIsStable;}
	// Just for test.
	bool setInMultiThreads() 
	{
		mRunFullActionsInBackground = false;
		mRunInThrdShell = true;
		return true;
	}
	
	bool setInBackGroud() 
	{
		mRunFullActionsInBackground = true;
		return true;
	}
	
	bool setInCurrentThread()
	{
		mRunFullActionsInBackground = false;
		mRunInThrdShell = false;
		return true;
	}


	virtual bool serializeFrom(
			const AosBuffPtr &buff, 
			const AosRundataPtr &rdata);

	virtual bool serializeTo(
			const AosBuffPtr &buff, 
			const AosRundataPtr &rdata);

protected:
	inline bool	checkFull(const AosRundataPtr &rdata)
	{
		if (mMaxNumRecords > 0)
		{
			if (size() > (int64_t)mMaxNumRecords) return true;
		}

		if (mMaxMemSize <= 0) return false;
		bool rslt = getMemSize() > mMaxMemSize;
		if (rslt)
			OmnMark;
		return (getMemSize() > mMaxMemSize);
	}
	
	inline bool handleFull(const AosRundataPtr &rdata)                 
	{
		if (mFullHandler)
		{
		     AosDataBlobObjPtr thisptr(this, false);
		     if (!mFullHandler->dataBlobFull(thisptr, rdata)) return true;
		}

		if (mFullActionsBeforeSort.size() <= 0 && mFullActionsAfterSort.size() <= 0) return true;

		return runFullActions(rdata);
	}
	virtual bool runFullActions(const AosRundataPtr &rdata) = 0;	
	bool config(const AosXmlTagPtr &conf, const AosRundataPtr &rdata);
	bool runMoreActions(const int idx, const int psize);
	bool runMoreEntrys(const int idx, const int psize);
	//vector<AosActionObjPtr>&  getCrtActions();
	
	bool runActionsInMultiThreads(
			const AosBuffArrayPtr &data,
			const vector<AosActionObjPtr> &actions_before_sort, 
			const vector<AosActionObjPtr> &actions_after_sort, 
			const AosRundataPtr &rdata);

	bool runActionsInMultiThreads(
			const AosBuffArrayPtr &data,
			const vector<AosActionObjPtr> &actions, 
			const AosRundataPtr &rdata);

	bool runActionsInBackground(
			const AosBuffArrayPtr &data,
			const vector<AosActionObjPtr> &actions_before_sort, 
			const vector<AosActionObjPtr> &actions_after_sort, 
			const AosRundataPtr &rdata);

	bool runActions(
			const AosBuffArrayPtr &data,
			const vector<AosActionObjPtr> &actions, 
			const AosRundataPtr &rdata);

	bool runActionsInCurrentThread(
			const AosBuffArrayPtr &data,
			const vector<AosActionObjPtr> &actions_before_sort, 
			const vector<AosActionObjPtr> &actions_after_sort, 
			const AosRundataPtr &rdata);
	
	bool runStartActionsInCurrentThread(
			const AosDataBlobObjPtr &blob,
			const vector<AosActionObjPtr> &actions_before_sort, 
			const AosRundataPtr &rdata);


	u64 determineNumPages(const u64 &size, const AosRundataPtr &rdata);
private:
	AosDataBlobFullHandlerPtr getFullHandler(
			const AosXmlTagPtr &def, 
			const AosRundataPtr &rdata);
	bool	startThrds();
	bool	processReq(const AosRunActionsReq &req);

};

#endif

