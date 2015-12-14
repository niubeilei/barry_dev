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
// 2012/10/16 Created by Ken
////////////////////////////////////////////////////////////////////////////
#if 0
#ifndef AOS_Job_JobSplitterDoc_h
#define AOS_Job_JobSplitterDoc_h


#include "Job/JobSplitter.h"
#include "TransClient/Ptrs.h"
#include "QueryUtil/QueryContext.h"
#include <vector>

class AosJobSplitterDoc : public AosJobSplitter
{
private:
	enum
	{
		eDefaultBlockSize = 10000000
	};
	struct ValueCond
	{
		OmnString   mValueType;
		OmnString   mValue1;
		OmnString   mValue2;
		AosOpr      mOpr;

		bool        mIsGroup;
		AosQueryContext::QueryKeyGroupingType   mKeyGroupType;
		AosQueryContext::QueryValueGroupingType mValueGroupType;

		bool        mNeedFilter;
		AosQueryContextObj::GroupFilterType  mFilterType;
		OmnString   mFilterValueType;
		OmnString   mFilterValue;
		AosOpr      mFilterOpr;

		ValueCond()
		{
			mValueType = "str";
			mOpr = eAosOpr_an;
			mIsGroup = false;
			mNeedFilter = false;
		}
	};
	OmnMutexPtr					mLock;
	OmnString					mDocScannerId;
	OmnString					mTaskSdocObjid;
	OmnString					mIILName;
	AosQueryContextObjPtr		mQueryContext;
	ValueCond					mValueCond;
	u64							mBlockSize;


public:
	AosJobSplitterDoc(const bool flag);
	~AosJobSplitterDoc();

	virtual bool splitTasks(
				map<OmnString, AosTaskDataObjPtr> &tasks,
				const AosRundataPtr &rdata);
	
	virtual AosJobSplitterObjPtr create(
				const AosXmlTagPtr &sdoc,
				const AosRundataPtr &rdata) const;

	bool runQuery(const AosRundataPtr &rdata);
	
private:
	bool	config(
				const AosXmlTagPtr &sdoc,
				const AosRundataPtr &rdata);

	bool    parseValCond(
				const AosXmlTagPtr &val_cond,
				const AosRundataPtr &rdata);
};
#endif

#endif
