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
// 03/25/2013	Created by Ketty 
////////////////////////////////////////////////////////////////////////////
#include "CounterUtil/CounterTrans.h"

#include "API/AosApi.h"
#include "CounterClt/Ptrs.h"
#include "SEInterfaces/CounterSvrObj.h"

AosCounterTrans::AosCounterTrans(const bool regflag)
:
AosCubicTrans(AosTransType::eCounter, regflag)
{
}


// will modify later.
AosCounterTrans::AosCounterTrans(
		const OmnString &counter_id,
		const AosCounterOperation::E &opr,
		const AosBuffPtr &buff,
		//const OmnString &cname, 
		//const int64_t &cvalue, 
		//const u64 &unitime,
		//const AosStatType::E statType,
		const bool need_save,
		const bool need_resp)
:
AosCubicTrans(AosTransType::eCounter, getDistid(counter_id), need_save, need_resp),
mCounterId(counter_id),
mOpr(opr),
//mCname(cname),
//mCvalue(cvalue),
//mUnitime(unitime),
//mStatType(statType),
mCont(buff)
{
}


AosCounterTrans::~AosCounterTrans()
{
}


bool
AosCounterTrans::serializeFrom(const AosBuffPtr &buff)
{
	bool rslt = AosCubicTrans::serializeFrom(buff);
	aos_assert_r(rslt, false);
	
	mCounterId = buff->getOmnStr("");
	mOpr = (AosCounterOperation::E)buff->getU32(0);
	//mCname = buff->getStr(""); 
	//mCvalue = buff->getU64(0); 
	//mUnitime = buff->getU64(0);
	//mStatType = (AosStatType::E)buff->getU32(0);
	u32 cont_len = buff->getU32(0);
	mCont = buff->getBuff(cont_len, true AosMemoryCheckerArgs);
	return true;
}


bool
AosCounterTrans::serializeTo(const AosBuffPtr &buff)
{
	bool rslt = AosCubicTrans::serializeTo(buff);
	aos_assert_r(rslt, false);
	
	buff->setOmnStr(mCounterId);
	buff->setU32(mOpr);
	buff->setU32(mCont->dataLen());	
	buff->setBuff(mCont);	
	//buff->setOmnStr(mCname);
	//buff->setU64(mCvalue);
	//buff->setU64(mUnitime);
	//buff->setU32(mStatType);
	return true;
}


AosTransPtr
AosCounterTrans::clone()
{
	return OmnNew AosCounterTrans(false);
}


bool
AosCounterTrans::proc()
{
	AosCounterSvrObjPtr svr = AosCounterSvrObj::getCounterSvr();
	aos_assert_r(svr, false);	
	
	AosCounterTransPtr thisptr(this, false);
	//svr->addTrans(thisptr, mCounterId, mOpr, mCont, mRdata);
	svr->addTrans(thisptr, mRdata);
	return true;
}


u32
AosCounterTrans::getDistid(const OmnString &counter_id)
{
	return AosGetHashKey(counter_id); 
}


