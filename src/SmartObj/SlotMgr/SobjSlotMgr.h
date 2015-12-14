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
// 12/18/2011	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SmartObj_SobjSlotMgr_SobjSlotMgr
#define Aos_SmartObj_SobjSlotMgr_SobjSlotMgr

#include "MultiLang/LangTermIds.h"
#include "Rundata/Rundata.h"
#include "SmartObj/SmartObj.h"
#include "Thread/ThreadedObj.h"
#include "XmlUtil/Ptrs.h"

class AosValue;

class AosSobjSlotMgr : public virtual AosSmartObj, public virtual OmnThreadedObj
{
	OmnDefineRCObject;

private:
	struct entry_t
	{
		int64_t start;
		int64_t size;

		entry_t(const int64_t &st, const int64_t &sz)
		:
		start(st),
		size(sz)
		{
		}
	};
	enum
	{
		eAddSlot = 100,
		eGetSlot = 101,
		eSave = 102,

		eDftMinStart = 0,
		eDftMaxStart = 0xffffffff,
		eDftMinSize  = 0,
		eDftMaxSize  = 0xffffffff,

		eStartIdx = 0,
		eSizeIdx = 1,
		eStatusIdx = 2
	};

	OmnMutexPtr		mLock;
	int64_t			mMinStart;
	bool			mMinStartFlag;
	int64_t			mMaxStart;
	bool			mMaxStartFlag;
	int64_t			mMinSize;
	bool			mMinSizeFlag;
	int64_t			mMaxSize;
	bool			mMaxSizeFlag;
	bool			mLogErrors;
	int64_t			mInvalidStart;
	int64_t			mInvalidSize;
	bool			mLogOperations;
	OmnString		mObjid;
	OmnString		mOprCtnrObjid;
	OmnString		mOprLogname;
	OmnString		mErrorCtnrObjid;
	OmnString		mErrorLogname;
	bool			mIsDirty;
	vector<AosSlotMgrSlabPtr>	mSlabs;
	vector<entry_t>	mEntries;
	AosXmlTagPtr	mDoc;

public:
	AosSobjSlotMgr(const bool flag);
	AosSobjSlotMgr(const OmnString &objid, const AosRundataPtr &rdata);
	AosSobjSlotMgr(const AosXmlTagPtr &doc, const AosRundataPtr &rdata);
	~AosSobjSlotMgr();

	virtual AosSmartObjPtr clone();
	virtual bool run(
					const int command, 
					AosValue &value,
					const AosXmlTagPtr &parms, 
					const AosRundataPtr &rdata);

	// ThreadedObj interface
	virtual bool	threadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread);
	virtual bool	signal(const int threadLogicId);
    virtual bool    checkThread(OmnString &err, const int thrdLogicId) const;

	inline bool isStartValid(
					const OmnString &operation,
					const int64_t &start, 
					const AosRundataPtr &rdata)
	{
		if ((mMinStartFlag && start < mMinStart) || (mMaxStartFlag && start > mMaxStart))
		{
			AosSetError(rdata, AOSLT_INVALID_START_POSITION);
			OmnAlarm << rdata->getErrmsg() << ". Start: " << start
				    << ". SmartObj: " << toString() << enderr;
			if (mLogErrors)
			{
				OmnString errmsg = AOSLT_INVALID_START_POSITION;
				errmsg << ". " << AOSLT_START_POSITION << ": " << start;
				logError(operation, AOSLT_ERROR, errmsg, rdata);
			}
			return false;
		}
		return true;
	}

	inline bool isSizeValid(
					const OmnString &operation,
					const int64_t &size, 
					const AosRundataPtr &rdata)
	{
		if ((mMinSizeFlag && size < mMinSize) || (mMaxSizeFlag && size > mMaxSize))
		{
			AosSetError(rdata, AOSLT_INVALID_SIZE);
			OmnAlarm << rdata->getErrmsg() << ". Size: " << size
				    << ". SmartObj: " << toString() << enderr;
			if (mLogErrors)
			{
				OmnString errmsg = AOSLT_INVALID_SIZE;
				errmsg << ". " << AOSLT_SIZE << ": " << size;
				logError(operation, AOSLT_ERROR, errmsg, rdata);
			}
			return false;
		}
		return true;
	}

	OmnString toString() const;
	void logError(const OmnString &operation, 
					const OmnString &status, 
					const OmnString &errmsg, 
					const AosRundataPtr &rdata);
	void logErrors(const OmnString &operation, 
					const OmnString &errmsg, 
					const AosRundataPtr &rdata)
	{
		logError(operation, AOSLT_ERROR, errmsg, rdata);
	}

	bool addEntry(const int64_t &start, 
				const int64_t &size, 
				const AosRundataPtr &rdata);

private:
	bool init(const AosXmlTagPtr &doc, const AosRundataPtr &rdata);
	bool addSlot(AosValue &value, const AosXmlTagPtr &parms, const AosRundataPtr &rdata);
	bool removeSlot(AosValue &value, const AosXmlTagPtr &parms, const AosRundataPtr &rdata);
	bool save(AosValue &value, const AosXmlTagPtr &parms, const AosRundataPtr &rdata);
	AosSlotMgrSlabPtr getSlab(const int64_t size, const AosRundataPtr &rdata);
	void logAdd(const int64_t &start, 
				const int64_t &size, 
				const AosXmlTagPtr &parms, 
				const AosRundataPtr &rdata);
	void logGet(const int64_t &start, 
				const int64_t &size, 
				const OmnString &status,
				const AosXmlTagPtr &parms, 
				const AosRundataPtr &rdata);
	bool getSlot(
				AosValue &value,
				const AosXmlTagPtr &parms, 
				const AosRundataPtr &rdata);
	bool removeEntry(const int64_t &start, 
				const int64_t &size, 
				const AosRundataPtr &rdata);
};

#endif

