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
// 07/28/2011	Created by Linda 
////////////////////////////////////////////////////////////////////////////
#include "DocTrans/BatchFixedFinishTrans.h"


#include "StorageEngine/Testers2/FixedLengthTesterMgr.h"
#include "Tester/TestMgr.h"
AosBatchFixedFinishTrans::AosBatchFixedFinishTrans(const bool regflag)
:
AosTaskTrans(AosTransType::eBatchFixedFinish, regflag)
{
}


AosBatchFixedFinishTrans::AosBatchFixedFinishTrans(
		const int phy,
		const int data_phy,
		const bool create,
		const bool need_save,
		const bool need_resp)
:
AosTaskTrans(AosTransType::eBatchFixedFinish, data_phy, need_save, need_resp),
mPhy(phy),
mDataPhy(data_phy),
mCreate(create)
{
}


AosBatchFixedFinishTrans::~AosBatchFixedFinishTrans()
{
}


bool
AosBatchFixedFinishTrans::serializeFrom(const AosBuffPtr &buff)
{
	bool rslt = AosTaskTrans::serializeFrom(buff);
	aos_assert_r(rslt, false);

	mPhy = buff->getInt(0);
	mDataPhy = buff->getInt(0);
	mCreate = buff->getU8(0);
	return true;
}


bool
AosBatchFixedFinishTrans::serializeTo(const AosBuffPtr &buff)
{
	bool rslt = AosTaskTrans::serializeTo(buff);
	aos_assert_r(rslt, false);

	buff->setInt(mPhy);
	buff->setInt(mDataPhy);
	buff->setU8(mCreate);
	return true;
}


AosTransPtr
AosBatchFixedFinishTrans::clone()
{
	return OmnNew AosBatchFixedFinishTrans(false);
}


bool
AosBatchFixedFinishTrans::proc()
{
	if (mCreate)
	{
		AosFixedLengthTesterMgr::setCreateFileFinished(mPhy, mDataPhy);
	}
	else
	{
		AosFixedLengthTesterMgr::docFinished(mPhy, mDataPhy);
	}

	mRdata->setOk();
	return true;
}
