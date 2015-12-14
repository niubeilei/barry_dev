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

#include "DfmUtil/DfmDocNorm.h"

#include "Thread/Mutex.h"
#include "Util/Buff.h"

AosDfmDocNorm::AosDfmDocNorm(const bool regflag)
:
AosDfmDoc(AosDfmDocType::eNormal, regflag)
{
}


AosDfmDocNorm::AosDfmDocNorm(const u64 docid)
:
AosDfmDoc(docid, AosDfmDocType::eNormal)
{
}

AosDfmDocNorm::~AosDfmDocNorm()
{
}


AosDfmDocPtr
AosDfmDocNorm::clone(const u64 docid)
{
	return OmnNew AosDfmDocNorm(docid);
}


bool
AosDfmDocNorm::serializeFrom(
		const AosBuffPtr &header_buff,
		u32 &body_seq,
		u64 &body_off)
{
	aos_assert_r(header_buff, false);
	
	getBodySeqOffFromBuff(header_buff, body_seq, body_off);
	return true;	
}


bool
AosDfmDocNorm::serializeTo(
		const AosBuffPtr &header_buff,
		const u32 body_seq,
		const u64 body_off)
{
	aos_assert_r(header_buff, false);
	
	setBodySeqOffToBuff(header_buff, body_seq, body_off);
	return true;
}


bool
AosDfmDocNorm::getBodySeqOffFromBuff(
		const AosBuffPtr &header_buff,
		u32 &body_seq,
		u64 &body_off)
{
	aos_assert_r(header_buff, false);
	header_buff->setCrtIdx(0);

	body_seq = header_buff->getU32(0);
	body_off = header_buff->getU64(0);
	return true;
}


bool
AosDfmDocNorm::setBodySeqOffToBuff(
		const AosBuffPtr &header_buff,
		const u32 body_seq,
		const u64 body_off)
{
	aos_assert_r(header_buff, false);
	header_buff->setCrtIdx(0);
	
	header_buff->setU32(body_seq);
	header_buff->setU64(body_off);
	return true;
}


