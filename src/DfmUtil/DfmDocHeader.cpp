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

#include "DfmUtil/DfmDocHeader.h"

#include "Thread/Mutex.h"
#include "Util/Buff.h"
#include "Util/MemoryChecker.h"


AosDfmDocHeader::AosDfmDocHeader(
		const u64 docid,
		const u32 body_seq,
		const u64 body_off,
		const AosBuffPtr &header_buff)
:
mDocid(docid),
mBodySeqno(body_seq),
mBodyOffset(body_off),
mHeaderBuff(header_buff)
{
}


AosDfmDocHeader::~AosDfmDocHeader()
{
}


bool
AosDfmDocHeader::isEmpty()
{
	return (mBodySeqno == 0 && mBodyOffset == 0);
}


bool
AosDfmDocHeader::isSameBodySeqOff(const AosDfmDocHeaderPtr &header)
{
	return mBodySeqno == header->mBodySeqno &&
		mBodyOffset == header->mBodyOffset;
}


void
AosDfmDocHeader::setBodySeqOff(const u32 seq, const u64 off)
{
	mBodySeqno = seq; 
	mBodyOffset = off; 
}


AosBuffPtr
AosDfmDocHeader::getHeaderBuff()
{
	return mHeaderBuff;	
}

AosDfmDocHeaderPtr
AosDfmDocHeader::copy()
{
	AosBuffPtr new_buff = OmnNew AosBuff(mHeaderBuff->dataLen() AosMemoryCheckerArgs);
	new_buff->setBuff(mHeaderBuff);

	return OmnNew AosDfmDocHeader(mDocid, mBodySeqno, mBodyOffset, new_buff); 
}

bool
AosDfmDocHeader::reset(
		const u32 body_seq,
		const u64 body_off,
		const AosBuffPtr &header_buff)
{
	mBodySeqno = body_seq;
	mBodyOffset = body_off;
	mHeaderBuff = header_buff;
	return true;
}

bool
AosDfmDocHeader::resetBuff(const AosBuffPtr &header_buff)
{
	mHeaderBuff = header_buff;
	return true;
}
