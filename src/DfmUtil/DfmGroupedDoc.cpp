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
// 01/23/2013 Created by Linda 
////////////////////////////////////////////////////////////////////////////
#include "DfmUtil/DfmGroupedDoc.h"

#include "Util/Buff.h"
#include "Util/MemoryChecker.h"


AosDfmGroupedDoc::AosDfmGroupedDoc(const bool regflag)
:
AosDfmDoc(AosDfmDocType::eGroupedDoc, regflag),
mSiteid(0),
mGroupId(0)
{
}


AosDfmGroupedDoc::AosDfmGroupedDoc(const u64 group_id)
:
AosDfmDoc(group_id, AosDfmDocType::eGroupedDoc),
mSiteid(0),
mGroupId(group_id)
{
}

AosDfmGroupedDoc::AosDfmGroupedDoc(
		const u32 siteid,
		const u64 group_id)
:
AosDfmDoc(group_id, AosDfmDocType::eGroupedDoc),
mSiteid(siteid),
mGroupId(group_id)
{
}

AosDfmGroupedDoc::~AosDfmGroupedDoc()
{
}	


AosDfmDocPtr
AosDfmGroupedDoc::clone(const u64 docid)
{
	return OmnNew AosDfmGroupedDoc(docid);
}

bool
AosDfmGroupedDoc::serializeFrom(
		const AosBuffPtr &header_buff,
		u32 &body_seq,
		u64 &body_off)
{
	//	eVersionOffset = 0,
	//	eSiteidOffset = eVersionOffset + 4,
	//	eGroupidOffset = eSiteidOffset + 4,
	//	eHeaderCommonInfoSize = eGroupidOffset + 8,	
	aos_assert_r(header_buff, false);
	header_buff->setCrtIdx(0);	
	
	header_buff->getU32(0);	// version.
	mSiteid = header_buff->getU32(0);
	mGroupId = header_buff->getU64(0); 
	
	getBodySeqOffFromBuff(header_buff, body_seq, body_off);
	return true;
}


bool
AosDfmGroupedDoc::serializeTo(
		const AosBuffPtr &header_buff,
		const u32 body_seq,
		const u64 body_off)
{
	//	eVersionOffset = 0,
	//	eSiteidOffset = eVersionOffset + 4,
	//	eGroupidOffset = eSiteidOffset + 4,
	//	eHeaderCommonInfoSize = eGroupidOffset + 8,	
	aos_assert_r(header_buff, false);
	header_buff->setCrtIdx(0);	

	header_buff->setU32(eCurrentVersion);
	header_buff->setU32(mSiteid);
	header_buff->setU64(mGroupId);
	
	setBodySeqOffToBuff(header_buff, body_seq, body_off);
	return true;
}


bool
AosDfmGroupedDoc::getBodySeqOffFromBuff(
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
AosDfmGroupedDoc::setBodySeqOffToBuff(
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


