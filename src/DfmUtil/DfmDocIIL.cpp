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
// 10/30/2012 Created by Ketty
////////////////////////////////////////////////////////////////////////////
#include "DfmUtil/DfmDocIIL.h"

#include "Thread/Mutex.h"
#include "Util/Buff.h"
#include "Util/MemoryChecker.h"


AosDfmDocIIL::AosDfmDocIIL(const bool regflag)
:
AosDfmDoc(AosDfmDocType::eIIL, regflag)
{
}

AosDfmDocIIL::AosDfmDocIIL(const u64 loc_did)
:
AosDfmDoc(loc_did, AosDfmDocType::eIIL),
mIILType(eAosIILType_Invalid),
mIILID(0),
mWordId(0),
mVersion(0),
mIsPersis(false),
mHitCount(0),
mNumDocs(0),
mFlag(0)
{
}

AosDfmDocIIL::AosDfmDocIIL(const u64 loc_did,
		const AosIILType iil_type,
		const u64 iilid,
		const u64 wordid,
		const u32 version,
		const bool is_persis,
		const u32 hit_count,
		const u32 num_docs,
		const char flag)
:
AosDfmDoc(loc_did, AosDfmDocType::eIIL),
mIILType(iil_type),
mIILID(iilid),
mWordId(wordid),
mVersion(version),
mIsPersis(is_persis),
mHitCount(hit_count),
mNumDocs(num_docs),
mFlag(false)
{
}


AosDfmDocIIL::~AosDfmDocIIL()
{
}

	
AosDfmDocPtr
AosDfmDocIIL::clone(const u64 docid)
{
	return OmnNew AosDfmDocIIL(docid);	
}

bool
AosDfmDocIIL::serializeFrom(
		const AosBuffPtr &header_buff,
		u32 &body_seq,
		u64 &body_off)
{
	//  eIILTypeOff = 0,
	// 	eIILIDOff = eIILTypeOff + 1,
	//	eWordIdOff = eIILIDOff + 8,
	//	eNumDocsOff = eWordIdOff + 8,
	//	eFlagOff = eNumDocsOff + 8,
	//	eHitCountOff = eFlagOff + 1,
	//	eVersionOff = eHitCountOff + 4,
	//	eHeaderCommonInfoSize = eVersionOff + 4,	// 34 byte.
	aos_assert_r(header_buff, false);
	
	header_buff->setCrtIdx(0);
	
	mIILType = (AosIILType)header_buff->getU8(0); 
	mIILID = header_buff->getU64(0);
	mWordId = header_buff->getU64(0);
	mNumDocs = header_buff->getI64(0);
	mFlag = header_buff->getChar(0);
	mHitCount = header_buff->getU32(0);	
	u32 ver = header_buff->getU32(0);
	mVersion = ver & ~ePersisBitOnVersionFlag;
	mIsPersis = ver & ePersisBitOnVersionFlag;
	
	getBodySeqOffFromBuff(header_buff, body_seq, body_off);
	return true;
}


bool
AosDfmDocIIL::serializeTo(
		const AosBuffPtr &header_buff,
		const u32 body_seq,
		const u64 body_off)
{
	//  eIILTypeOff = 0,
	// 	eIILIDOff = eIILTypeOff + 1,
	//	eWordIdOff = eIILIDOff + 8,
	//	eNumDocsOff = eWordIdOff + 8,
	//	eFlagOff = eNumDocsOff + 8,
	//	eHitCountOff = eFlagOff + 1,
	//	eVersionOff = eHitCountOff + 4,
	//	eHeaderCommonInfoSize = eVersionOff + 4,	// 34 byte.
	aos_assert_r(header_buff, false);
	
	header_buff->setCrtIdx(0);
	
	header_buff->setU8((u8)mIILType);
	header_buff->setU64(mIILID);
	header_buff->setU64(mWordId);
	header_buff->setI64(mNumDocs);
	header_buff->setChar(mFlag);
	header_buff->setU32(mHitCount);

	u32 ver = mVersion;
	if(mIsPersis)
	{
		ver = ver | ePersisBitOnVersionFlag;
	}
	header_buff->setU32(ver);
	aos_assert_r(header_buff->getCrtIdx() == eHeaderCommonInfoSize, false);
	
	setBodySeqOffToBuff(header_buff, body_seq, body_off);
	return true;
}


bool
AosDfmDocIIL::getBodySeqOffFromBuff(
		const AosBuffPtr &header_buff,
		u32 &body_seq,
		u64 &body_off)
{
	aos_assert_r(header_buff, false);
	header_buff->setCrtIdx(eHeaderCommonInfoSize);

	header_buff->getU32(0);		// body size.
	body_seq = header_buff->getU32(0);
	body_off = header_buff->getU64(0);
	return true;
}


bool
AosDfmDocIIL::setBodySeqOffToBuff(
		const AosBuffPtr &header_buff,
		const u32 body_seq,
		const u64 body_off)
{
	aos_assert_r(header_buff, false);
	header_buff->setCrtIdx(eHeaderCommonInfoSize);
	
	header_buff->setU32(0);		// body size.
	header_buff->setU32(body_seq);
	header_buff->setU64(body_off);
	return true;
}


