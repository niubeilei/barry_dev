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
// 2013/11/01 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "Dataset/DatasetMulti.h"

#include "Rundata/Rundata.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/Ptrs.h"
#include "Thread/Mutex.h"
#include "SEInterfaces/DataRecordObj.h"


AosDatasetMulti::AosDatasetMulti()
:
AosDataset(AOS_JIMOTYPE_DATASET, 0)
{
	mDatasetList.clear();
	mPageSize = 20;
	mTotalValues = 0;
	mNumValues = 0;
}


AosDatasetMulti::~AosDatasetMulti()
{
}

//
//Union DatasetMulti without removing duplicate
//records
//
bool
AosDatasetMulti::dataUnionAll(
		const AosRundataPtr &rdata,
		AosDatasetObjPtr &ds)
{
		aos_assert_r(ds, false);

		mIsUnionAll = true;
		mDatasetList.push_back(ds);
	    return true;
}

//
//Union DatasetMulti by removing duplicate records
//
bool
AosDatasetMulti::dataUnion(
		const AosRundataPtr &rdata,
		AosDatasetObjPtr &ds)
{
		aos_assert_r(ds, false);

		mIsUnionAll = false;
		mDatasetList.push_back(ds);
	    return true;
}

bool 
AosDatasetMulti::nextRecordset(
		const AosRundataPtr &rdata, 
		AosRecordsetObjPtr &recordset)
{
	aos_assert_r(mDatasetList.size() > 0, false);

	//init member var and params
	mUnionSet.clear();
	recordset = 0;

	//init local variables
	AosBuffDataPtr buffData;
	bool rslt = true;
	u32 idx = 0;
	bool rsSetFlag = false;
	AosDatasetObjPtr ds = mDatasetList[idx];

	AosRecordsetObjPtr rs;
	AosDataRecordObj* rcd;
	AosDataRecordObjPtr rcdPtr;
	//page size == 0 means no limitation
	while (mNumValues < mPageSize || mPageSize == 0)
	{
		rslt = ds->nextRecordset(rdata, rs);
		aos_assert_r(rslt, false);
		if (rs && !rsSetFlag)
		{
			recordset = rs->clone(rdata.getPtr());
			recordset->reset();
			rsSetFlag = true;
		}

		if (AosRecordsetObj::checkEmpty(rs))
		{
			idx++;
			if (idx >= mDatasetList.size())
				break;

			ds = mDatasetList[idx];
			continue;
		}
	
		while (1)
		{
			rslt = rs->nextRecord(rdata.getPtrNoLock(), rcd);
			aos_assert_r(rslt, false);
			if (!rcd) break;

			if (!mIsUnionAll)
			{
				//need to remove duplicate records
				OmnString ss(rcd->getData(rdata.getPtrNoLock()), rcd->getMemoryLen());
				aos_assert_r(ss != "", false);
				if (mUnionSet.count(ss) > 0)
					continue;
					
				mUnionSet.insert(ss);
			}

			//append the record to recordset
			rcdPtr = rcd;
			recordset->appendRecord(rdata.getPtr(), rcdPtr, buffData);
			mNumValues++;
			if (mNumValues >= mPageSize && mPageSize != 0)
				break;
		}
	}
	// JIMODB-1203
	mTotalValues = mNumValues;

	return true;
}

///////////////////////////////////////////
//   Helper methods
///////////////////////////////////////////
bool
AosDatasetMulti::getRsltInfor(
		AosXmlTagPtr &infor,
		const AosRundataPtr &rdata)
{
	OmnString str = "<Contents";
	str << " total=\"" << mTotalValues << "\""
		<< " start_idx=\"" << 0 << "\""
		<< " num=\"" << mNumValues << "\""
		<< "/>";

	infor = AosXmlParser::parse(str AosMemoryCheckerArgs);
	aos_assert_r(infor, false);

#if 0
	//don't use query time here. Use JQLStmtQuery's run time
	OmnString time_str;
	time_str << (mQueryFinishTime - mQueryStartTime) / 1000.0 << "ms";
	infor->setAttr("time", time_str);
#endif

	return true;
}

