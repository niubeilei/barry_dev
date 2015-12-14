////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: Statistics.h
// Description:
//   
//
// Modification History:
// 2014/08/06 Created by Ketty
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Vector2DQryRslt_Vt2dQryRslt_h
#define Vector2DQryRslt_Vt2dQryRslt_h

#include "Vector2DUtil/Ptrs.h"
#include "Vector2DUtil/Vt2dRecord.h"

using namespace std;

class AosVt2dQryRslt: public OmnRCObject 
{
	OmnDefineRCObject;

private:
	vector<AosVt2dRecordPtr>	mRecords;
	vector<AosVt2dRecord *>		mRecordsRaw;
	vector<u64>					mStatDocids;
	map<u64, u32> 				mRcdIdx;

protected:
	u32		mMeasureValueSize;
	u32		mExpectMaxValueNum;

public:
	AosVt2dQryRslt(const u32 measure_value_size)
	:
	mMeasureValueSize(measure_value_size),
	mExpectMaxValueNum(0)
	{
	}
	
	void	setMaxTimeValueNum(const u32 num){ mExpectMaxValueNum = num; };
	
	bool	appendVt2dRecord(const u32 stat_doc_idx, const u64 stat_docid)
	{
		aos_assert_r(mStatDocids.size() == stat_doc_idx, false);
		mStatDocids.push_back(stat_docid);

		AosVt2dRecordPtr new_rcd = OmnNew AosVt2dRecord(stat_docid, mMeasureValueSize, mExpectMaxValueNum);
		mRecords.push_back(new_rcd);
		mRecordsRaw.push_back(new_rcd.getPtr());
		return true;
	}

	AosVt2dRecord * getVt2dRecordRaw(
			const u32 stat_doc_idx,
			const u64 stat_docid)
	{
		aos_assert_r(stat_doc_idx < mStatDocids.size(), 0);
		aos_assert_r(stat_doc_idx < mRecords.size(), 0);
		aos_assert_r(mStatDocids[stat_doc_idx] == stat_docid, 0);
		
		return mRecordsRaw[stat_doc_idx];
	}

	//vector<AosVt2dRecordPtr> & getAllRcds(){ return mRecords; };
	u32 getRcdNum(){ return mRecords.size(); };

	AosVt2dRecordPtr getVt2dRecordByRcdIdx(const u32 rcd_idx)
	{
		aos_assert_r(rcd_idx < mRecords.size(), 0);
		return mRecords[rcd_idx];
	}

};

#endif
