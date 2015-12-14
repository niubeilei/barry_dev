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
// 2013/04/29Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "BitmapTrans/TransSendInstrs.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "BitmapTrans/Ptrs.h"
#include "Rundata/Rundata.h"
#include "SEInterfaces/BitmapInstrRunnerObj.h"
#include "Thread/Mutex.h"
#include "Thread/CondVar.h"

AosTransSendInstrs::AosTransSendInstrs(const bool regflag)
:
AosBitmapTrans(AosTransType::eSendBitmapInstrs, regflag),
mExecutorID(0)
{
}


AosTransSendInstrs::AosTransSendInstrs(
		const AosRundataPtr &rdata, 
		const AosBuffPtr &bitmap_buff,
		const AosBuffPtr &instr_buff,
		//const int phy_id,
		const u32 cube_id,
		const u64 &executor_id)
:
AosBitmapTrans(AosTransType::eSendBitmapInstrs, cube_id, true, false, true),
mExecutorID(executor_id),
mBitmapsBuff(bitmap_buff),
mInstrBuff(instr_buff)
{
}


AosTransSendInstrs::~AosTransSendInstrs()
{
}


bool
AosTransSendInstrs::serializeTo(const AosBuffPtr &buff)
{
	// Its format is:
	// 		buff_len		u64
	// 		buff			AosBuff
	// 		executor_id		u64
	bool rslt = AosBitmapTrans::serializeTo(buff);
	aos_assert_r(rslt, false);

	buff->setU64(mExecutorID);
	if(mBitmapsBuff && mBitmapsBuff->dataLen() > 0)
	{
		buff->setU64(mBitmapsBuff->dataLen());
		buff->setBuff(mBitmapsBuff);
	}
	else
	{
		buff->setU64(0);
	}
	
	buff->setU64(mInstrBuff->dataLen());
	buff->setBuff(mInstrBuff);
	return true;
}


bool
AosTransSendInstrs::serializeFrom(const AosBuffPtr &buff)
{
	// Its format is:
	// 		buff_len		u64
	// 		buff			AosBuff
	// 		executor_id		u64
	
	bool rslt = AosBitmapTrans::serializeFrom(buff);
	aos_assert_r(rslt, false);

	mExecutorID = buff->getU64(0);
	if (mExecutorID == 0)
	{
		OmnAlarm << "missing executor id" << enderr;
		return false;
	}

	u64 buff_len = buff->getU64(0);
	if(buff_len)
	{
		mBitmapsBuff = buff->getBuff(buff_len, true AosMemoryCheckerArgs);
		aos_assert_r(mBitmapsBuff, false);
	}
	else
	{
		mBitmapsBuff = 0;
	}
	
	buff_len = buff->getU64(0);
	aos_assert_r(buff_len, false);

	mInstrBuff = buff->getBuff(buff_len, true AosMemoryCheckerArgs);
	aos_assert_r(mInstrBuff, false);

	return true;
}


AosTransPtr
AosTransSendInstrs::clone()
{
	return OmnNew AosTransSendInstrs(false);
}


bool 
AosTransSendInstrs::proc()
{
	return AosBitmapInstrRunnerObj::addTransStatic(this);
}



int
AosTransSendInstrs::getSerializeSize() const
{
	OmnShouldNeverComeHere;
	return -1;
}

bool
AosTransSendInstrs::respCallBack()
{
	return true;
}

