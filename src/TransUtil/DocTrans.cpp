
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
//
// Modification History:
// 10/05/2011	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "TransUtil/DocTrans.h"

#include "API/AosApi.h"
#include "Rundata/Rundata.h"
#include "TransUtil/TransProcThrd.h"
#include "Util/Buff.h"
#include "XmlUtil/XmlDoc.h"

AosTransProcThrdPtr 	AosDocTrans::smProcThrd = OmnNew AosTransProcThrd("docTrans", 16);

AosDocTrans::AosDocTrans(
		const AosTransType::E type, 
		const bool reg_flag)
:
AosCubicTrans(type, reg_flag),
mDistId(0),
mSnapshotId(0)
{
}


AosDocTrans::AosDocTrans(
		const AosTransType::E type,
		const u64 &docid,
		const bool need_save,
		const bool need_resp,
		const u64 snap_id)
:
AosCubicTrans(type, AosXmlDoc::getOwnDocid(docid), need_save, need_resp),
mDistId(docid),
mSnapshotId(snap_id)
{
}


AosDocTrans::AosDocTrans(
		const AosTransType::E type,
		const u32 vid,
		const bool is_cube,
		const bool need_save,
		const bool need_resp,
		const u64 snap_id)
:
AosCubicTrans(type, vid, is_cube, need_save, need_resp),
mDistId(vid),
mSnapshotId(snap_id)
{
}


AosDocTrans::~AosDocTrans()
{
}


bool
AosDocTrans::serializeTo(const AosBuffPtr &buff)
{
	AosCubicTrans::serializeTo(buff);
	buff->setU64(mSnapshotId);
	return true;
}


bool
AosDocTrans::serializeFrom(const AosBuffPtr &buff)
{
	AosCubicTrans::serializeFrom(buff);
	mSnapshotId = buff->getU64(0);
	return true;
}


bool	
AosDocTrans::directProc()
{
	u32 idx = mDistId % smProcThrd->getNumThrds();
	AosTransPtr thisptr(this, false);
	return smProcThrd->addRequest(idx, thisptr);	
}

