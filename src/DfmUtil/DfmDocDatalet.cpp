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

#include "DfmUtil/DfmDocDatalet.h"

#include "Util/Buff.h"
#include "Util/MemoryChecker.h"


AosDfmDocDatalet::AosDfmDocDatalet(const bool regflag)
:
AosDfmDoc(AosDfmDocType::eDatalet, regflag),
mSiteid(0)
{
}

AosDfmDocDatalet::AosDfmDocDatalet(const u64 docid)
:
AosDfmDoc(docid, AosDfmDocType::eDatalet),
mSiteid(0)
{
}


AosDfmDocDatalet::AosDfmDocDatalet(
		const u64 loc_did,
		const u64 docid,
		const u32 siteid)
:
AosDfmDoc(loc_did, AosDfmDocType::eDatalet),
mSiteid(siteid)
{
}


AosDfmDocDatalet::~AosDfmDocDatalet()
{
}	


AosDfmDocPtr
AosDfmDocDatalet::clone(const u64 docid)
{
	return OmnNew AosDfmDocDatalet(docid);
}

bool
AosDfmDocDatalet::serializeFrom(
		const AosBuffPtr &header_buff,
		u32 &body_seq,
		u64 &body_off)
{
	//	eVersionOff = 0,
	//	eSiteidOff = eVersionOff + 4,
	//	eDocidOff = eSiteidOff + 4,
	aos_assert_r(header_buff, false);
	
	header_buff->setCrtIdx(0);
	
	header_buff->getU32(0);	// version.
	mSiteid = header_buff->getU32(0);
	mDocDid = header_buff->getU64(0);

	//header_buff->getU32(0); //size.
	
	getBodySeqOffFromBuff(header_buff, body_seq, body_off);
	return true;
}

bool
AosDfmDocDatalet::serializeTo(
		const AosBuffPtr &header_buff,
		const u32 body_seq,
		const u64 body_off)
{
	//	eVersionOff = 0,
	//	eSiteidOff = eVersionOff + 4,
	//	eDocidOff = eSiteidOff + 4,
	aos_assert_r(header_buff, false);
	
	header_buff->setCrtIdx(0);
	
	header_buff->setU32(eCurrentVersion);
	header_buff->setU32(mSiteid);
	header_buff->setU64(mDocDid);
	
	//header_buff->setU32(0); //size.

	setBodySeqOffToBuff(header_buff, body_seq, body_off);
	return true;
}

bool
AosDfmDocDatalet::getBodySeqOffFromBuff(
		const AosBuffPtr &header_buff,
		u32 &body_seq,
		u64 &body_off)
{
	aos_assert_r(header_buff, false);
	header_buff->setCrtIdx(eHeaderCommonInfoSize);

	body_seq = header_buff->getU32(0);
	body_off = header_buff->getU64(0);
	return true;
}


bool
AosDfmDocDatalet::setBodySeqOffToBuff(
		const AosBuffPtr &header_buff,
		const u32 body_seq,
		const u64 body_off)
{
	aos_assert_r(header_buff, false);
	header_buff->setCrtIdx(eHeaderCommonInfoSize);
	
	header_buff->setU32(body_seq);
	header_buff->setU64(body_off);
	return true;
}


