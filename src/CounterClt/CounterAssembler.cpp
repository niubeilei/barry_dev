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
// This class is used to assemble statistics requests. When it is full, 
// it sends the accumulated requests to the remote server. 
//
// Note that in the current implementations, requests are not saved, which
// means that it is unsafe. 
//
// Assemblers are used to cache requests. For requests that need responses, 
// assmeblers should not be used.
//
// Modification History:
// 03/30/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "CounterClt/CounterAssembler.h"

#include "API/AosApi.h"
#include "CounterClt/CounterClt.h"
#include "CounterUtil/CounterTrans.h"
#include "CounterUtil/CounterUtil.h"
#include "CounterUtil/CounterOperations.h"
#include "Util/Buff.h"

u64 AosCounterAssembler::smMaxBuffSize = AosCounterAssembler::eDftMaxBuffSize;


AosCounterAssembler::AosCounterAssembler()
{
	/*
	mNumVirtuals = AosGetNumCubes();
	if (mNumVirtuals <= 0)
	{
		OmnThrowException("No virtuals");
		return;
	}

	mNumPhysicals = AosGetNumPhysicals();
	if (mNumPhysicals <= 0)
	{
		OmnThrowException("No physicals");
		return;
	}

	for (int i=0; i<mNumPhysicals; i++)
	{
		mBuffs.push_back(OmnNew AosBuff(AosMemoryCheckerArgsBegin));
		mLocks.push_back(OmnNew OmnMutex());
	}
	*/
}


AosCounterAssembler::~AosCounterAssembler()
{
}

void
AosCounterAssembler::init(const u64 &num_phy, const u64 &num_vir)
{
	mNumVirtuals = AosGetNumCubes();
	// Chen Ding, 06/08/2012
	// if (mNumVirtuals <= 0)
	// {
	// 	OmnThrowException("No virtuals");
	// 	return;
	// }

	// Ketty 2012/11/02
	mNumPhysicals = AosGetNumPhysicals();

	// if (mNumPhysicals <= 0)
	// {
	// 	OmnThrowException("No physicals");
	// 	return;
	// }

	for (int i=0; i<mNumPhysicals; i++)
	{
		mBuffs.push_back(OmnNew AosBuff(AosMemoryCheckerArgsBegin));
		mLocks.push_back(OmnNew OmnMutex());
	}
}


bool
AosCounterAssembler::config(const AosXmlTagPtr &conf)
{
	if (!conf) return true;
	AosXmlTagPtr tag = conf->getFirstChild("couner_assembler");
	if (!tag) return true;
	smMaxBuffSize = tag->getAttrU64("size_to_send", 0);
	if (smMaxBuffSize <= 0) smMaxBuffSize = eDftMaxBuffSize;
	return true;
}


bool 
AosCounterAssembler::addCounter(
		const OmnString &counter_id,
		const OmnString &cname, 
		const int64_t &cvalue, 
		const u64 &unitime,
		const AosStatType::E statType,
		const AosCounterOperation::E &operation,
		const AosRundataPtr &rdata)
{
	// It calculates the hash key of 'key', and gets the virtual ID.
	// It then adds the entry [key, member, unitime, cvalue] to 
	// the corresponding buff.
	//
	// The format for each request is:
	// 		counter id			OmnString
	// 		operation			u8
	// 		cname				OmnString
	// 		cvalue				int64_t
	// 		unitime				u64
	// 		stattype			u8
	//
	aos_assert_rr(mNumVirtuals > 0, rdata, false);
	aos_assert_rr(mNumPhysicals > 0, rdata, false);
	aos_assert_rr(counter_id != "", rdata, false);
	aos_assert_rr(cname != "", rdata, false);

	// Ketty 2013/03/25
	/*
	u64 hashkey = AosGetHashKey(counter_id);
	u32 vid = (hashkey % (u32)mNumVirtuals);
	u32 pid = vid % (u32)mNumPhysicals;
	AosBuffPtr buff = mBuffs[pid];
	OmnMutexPtr lock = mLocks[pid];
	aos_assert_rr(buff, rdata, false);

	
	lock->lock();
	buff->setOmnStr(counter_id);
	//buff->setU8(AosCounterOperation::eAddCounter);
	buff->setU8(operation);
	buff->setOmnStr(cname);
	buff->setI64(cvalue);
	buff->setU64(unitime);
	buff->setU8(statType);
	
	int len = buff->dataLen();
	//if (len > 0 && (u64)len > smMaxBuffSize)
	if (len > 0 && (u64)len > smMaxBuffSize)
	{
		AosCounterClt::getSelf()->sendBuffTrans(vid, buff, rdata);
		buff->reset();
		buff->setDataLen(0);
	}
	lock->unlock();
	*/

	//AosTransPtr trans = OmnNew AosAddCounterTrans(
	//		counter_id, cname, cvalue, unitime, statType);
	
	AosBuffPtr buff = OmnNew AosBuff(20 AosMemoryCheckerArgs);
	buff->setOmnStr(cname);
	buff->setI64(cvalue);
	buff->setU64(unitime);
	buff->setU8(statType);
	AosTransPtr trans = OmnNew AosCounterTrans(
			counter_id, operation, buff, false, false);
	bool rslt = AosSendTrans(rdata, trans);
	aos_assert_r(rslt, false);
	return true;
}


bool 
AosCounterAssembler::addCounter2(
		const AosCounterOperation::E &operation,
		const OmnString &counter_id,
		const OmnString &cname, 
		const int64_t &cvalue, 
		const u64 &unitime,
		const AosTimeGran::E time_gran,
		const u16 &entry_type,
		const AosRundataPtr &rdata)
{
	// It calculates the hash key of 'key', and gets the virtual ID.
	// It then adds the entry [key, member, unitime, cvalue] to 
	// the corresponding buff.
	//
	// The format for each request is:
	// 		counter id			OmnString
	// 		operation			u8
	// 		cname				OmnString
	// 		cvalue				int64_t
	// 		unitime				u64
	// 		timegran			u8
	// 		entrytype			u16	
	//
	aos_assert_rr(mNumVirtuals > 0, rdata, false);
	aos_assert_rr(mNumPhysicals > 0, rdata, false);
	aos_assert_rr(counter_id != "", rdata, false);
	aos_assert_rr(cname != "", rdata, false);

	// Ketty 2013/03/28
	/*
	u64 hashkey = AosGetHashKey(counter_id);
	u32 vid = (hashkey % (u32)mNumVirtuals);
	u32 pid = vid % (u32)mNumPhysicals;
	AosBuffPtr buff = mBuffs[pid];
	OmnMutexPtr lock = mLocks[pid];
	aos_assert_rr(buff, rdata, false);

	lock->lock();
	buff->setOmnStr(counter_id);
	buff->setU8(operation);
	buff->setOmnStr(cname);
	buff->setI64(cvalue);
	buff->setU64(unitime);
	buff->setU8(time_gran);
	buff->setU16(entry_type);
	
	int len = buff->dataLen();
	if (len > 0 && (u64)len > smMaxBuffSize)
	{
		AosCounterClt::getSelf()->sendBuffTrans(vid, buff, rdata);
		buff->reset();
		buff->setDataLen(0);
	}
	lock->unlock();
	*/
	
	AosBuffPtr buff = OmnNew AosBuff(20 AosMemoryCheckerArgs);
	buff->setOmnStr(cname);
	buff->setI64(cvalue);
	buff->setU64(unitime);
	buff->setU8(time_gran);
	buff->setU16(entry_type);
	
	AosTransPtr trans = OmnNew AosCounterTrans(
			counter_id, operation, buff, false, false);
	bool rslt = AosSendTrans(rdata, trans);
	aos_assert_r(rslt, false);
	
	return true;
}

