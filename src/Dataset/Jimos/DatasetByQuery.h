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
// 2014/04/29 Created by Ken Lee
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_Dataset_DatasetByQuery_h
#define Aos_Dataset_DatasetByQuery_h

#include "Dataset/Dataset.h"

#include "QueryUtil/QrUtil.h"
#include "SEInterfaces/ConditionObj.h"
#include "SEInterfaces/DataRecordObj.h"
#include "Thread/Mutex.h"


class AosDatasetByQuery : public AosDataset
{
	OmnDefineRCObject;

	enum
	{
		eDftPsize = 20
	};

	struct comp
	{
		vector< pair<u32, bool> >	mCrtSortField;
		bool operator () (
				const vector<AosValueRslt> &l,
				const vector<AosValueRslt> &r);

		int compare(
				const vector<AosValueRslt> &l,
				const vector<AosValueRslt> &r);
	};

	struct groupByAggregate
	{
		OmnString	mType;
		int			mIdx;
		bool		mDistinct;
	};

	struct fieldDef
	{
		AosExprObjPtr	mOExpr;
		OmnString		mOname;
		OmnString		mCname;
		int				mCidx;
	};

protected:
	u64										mQueryStartTime;
	u64										mQueryFinishTime;
	OmnMutexPtr								mLock;

	AosConditionObjPtr						mWhereCond;
	AosConditionObjPtr						mHavingCond;

	AosDataRecordObjPtr						mDataRecord;
	vector<fieldDef>						mFieldDef;

	bool									mIsIndexQuery;
	bool									mGetTotal;
	i64										mStartIdx;
	i64										mPageSize;
	i64										mTotalValues;
	i64										mNumValues;
	i64										mNeedIgnoreNum;

	bool									mGroupBy;
	bool									mGroupByNeedSort;
	vector< pair<u32, bool> >				mGroupByField;
	vector<groupByAggregate>				mGroupByAggregate;

	bool									mOrderBy;
	vector< pair<u32, bool> >				mOrderByField;

	bool									mUsingFieldValues;
	vector<AosExprObjPtr>					mFieldNameExprs;
	vector< vector<AosValueRslt> >			mFieldValues;
	
	bool									mDataReaded;

	//Phil 2015-09-03, add input dataset for subquery
	AosDatasetObjPtr	mInputDataset;

public:
	AosDatasetByQuery(const OmnString &type, const int version);
	~AosDatasetByQuery();

	/*
	void setInput(const AosDatasetObjPtr &inputDataset)
	{
		mInputDataset = inputDataset;
	}
	*/

	i64 getTotalValues() { return mTotalValues; }
	i64 getNumValues() { return mNumValues; }
	u64 getQueryStartTime() { return mQueryStartTime; }
	u64 getQueryFinishTime() { return mQueryFinishTime; }
	i64 getPageSize() { return mPageSize; }
	
	void setScanDataset(AosDatasetObjPtr inputDataset){mInputDataset = inputDataset;};

	// jimodb-1375
	void    setPageSize(u64 num) { mPageSize = num; }

protected:
	virtual bool	config(
						const AosRundataPtr &rdata,
						const AosXmlTagPtr &def);

	virtual bool	initUnIndexQuery(
						const AosRundataPtr &rdata,
						const AosXmlTagPtr &def);

	virtual bool	checkIsIndexQuery(
						const AosRundataPtr &rdata,
						const AosXmlTagPtr &def) = 0;

	virtual bool	parseGroupBy( 
						const AosRundataPtr &rdata,
						const AosXmlTagPtr &def);

	virtual bool	parseOrderBy( 
						const AosRundataPtr &rdata,
						const AosXmlTagPtr &def);

	virtual bool	groupby(const AosRundataPtr &rdata);

	virtual bool	orderby(const AosRundataPtr &rdata);

	virtual bool	groupbyAggregate(
						const u32 start_idx,
						const u32 end_idx,
						vector<AosValueRslt> &vv,
						const AosRundataPtr &rdata);

	virtual bool	checkHaving(
						vector<AosValueRslt> &vv,
						const AosRundataPtr &rdata);
	virtual bool	getRecord(const OmnString &name, AosDataRecordObjPtr &record);
	virtual bool	reset(const AosRundataPtr &rdata);
	virtual bool getRecords(vector<AosDataRecordObjPtr> &records);

protected:
	bool gatherDocids(const AosRundataPtr &rdata);	// Chen Ding, 2015/01/28
};
#endif



