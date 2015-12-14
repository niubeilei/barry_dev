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
// 2013/02/15 Created by Chen Ding 
////////////////////////////////////////////////////////////////////////////
#include "DfmUtil/DfmDocBitmap.h"

#include "Thread/Mutex.h"
#include "Util/Buff.h"
#include "Util/MemoryChecker.h"


bool AosDfmDocBitmap::smShowLog = true;

AosDfmDocBitmap::AosDfmDocBitmap(const bool regflag)
:
AosDfmDoc(AosDfmDocType::eBitmap, regflag),
mBitmapId(0),
mNodeLevel(-1),
mIILLevel(-1)
{
}

AosDfmDocBitmap::AosDfmDocBitmap(
		const u64 &docid, 
		const int node_level,
		const int iil_level)
:
AosDfmDoc(docid, AosDfmDocType::eBitmap),
mBitmapId(0),
mNodeLevel(node_level),
mIILLevel(iil_level)
{
}


AosDfmDocBitmap::~AosDfmDocBitmap()
{
}

	
AosDfmDocPtr
AosDfmDocBitmap::clone(const u64 docid)
{
	return OmnNew AosDfmDocBitmap(docid);	
}


bool
AosDfmDocBitmap::serializeFrom(
		const AosBuffPtr &header_buff,
		u32 &body_seq,
		u64 &body_off)
{
	// Header format is:
	// 	body seqno		u32
	// 	body offset		u64
	// 	bitmap id		u64
	// 	node_level		int
	// 	iil_level		int
	aos_assert_r(header_buff, false);
	
	header_buff->setCrtIdx(eBitmapIdOff);
	
	mBitmapId = header_buff->getU64(0);
	mNodeLevel = header_buff->getInt(-1);
	mIILLevel = header_buff->getInt(-1);

	getBodySeqOffFromBuff(header_buff, body_seq, body_off);
	return true;
}


bool
AosDfmDocBitmap::serializeTo(
		const AosBuffPtr &header_buff,
		const u32 body_seq,
		const u64 body_off)
{
	aos_assert_r(header_buff, false);

	header_buff->setCrtIdx(eBitmapIdOff);
	
	header_buff->setU64(mBitmapId);
	header_buff->setInt(mNodeLevel);
	header_buff->setInt(mIILLevel);

	setBodySeqOffToBuff(header_buff, body_seq, body_off);
	return true;
}

bool
AosDfmDocBitmap::getBodySeqOffFromBuff(
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
AosDfmDocBitmap::setBodySeqOffToBuff(
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


