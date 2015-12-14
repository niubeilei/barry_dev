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
// 2014/01/22 Created by Ketty
////////////////////////////////////////////////////////////////////////////

#include "Vector2DQryRslt/Vt2dTimeGrpby.h"

AosVt2dTimeGrpby::AosVt2dTimeGrpby()
{
}

AosVt2dTimeGrpby::AosVt2dTimeGrpby(
		const u32 vt2d_idx,
		const AosMeasureValueMapper &value_mapper)
:
AosVt2dQryRsltProc(vt2d_idx, value_mapper)
{
}


AosVt2dTimeGrpby::~AosVt2dTimeGrpby()
{
}


bool
AosVt2dTimeGrpby::appendValueToRcd(
		AosRundata *rdata,
		AosVt2dRecord* rcd,
		const i64 &new_time_value,
		char * new_value,
		const u32 new_value_len)
{
	bool rslt;
	aos_assert_r(rcd, false);
	int64_t last_time = rcd->getLastTime();
	aos_assert_r(last_time <= new_time_value, false);

	if(last_time < new_time_value)
	{
		bool is_valid;
		rslt = mValueMapper.getOutputData(rdata, new_value, new_value_len,
			mTmpOutputData, mValueMapper.mOutputDef.mValueSize, is_valid);
		aos_assert_r(rslt, false);
		if(!is_valid)	return true;

		return rcd->appendValue(new_time_value, mTmpOutputData,
				mValueMapper.mOutputDef.mValueSize);
	}
	
	// need group by.
	char *old_value = 0;
	u32 old_value_len;
	rslt = rcd->getLastValue(old_value, old_value_len, false);

	mValueMapper.updateOutputData(rdata, new_value, new_value_len,
			old_value, old_value_len);
	return true;
}

