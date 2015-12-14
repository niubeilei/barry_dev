
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
#include "TransUtil/BitmapTrans.h"

#include "API/AosApi.h"
#include "Rundata/Rundata.h"
#include "TransUtil/TransProcThrd.h"
#include "Util/Buff.h"
#include "XmlUtil/XmlDoc.h"

AosTransProcThrdPtr 	AosBitmapTrans::smProcThrd = OmnNew AosTransProcThrd("docTrans");

AosBitmapTrans::AosBitmapTrans(
		const AosTransType::E type, 
		const bool reg_flag)
:
AosCubicTrans(type, reg_flag),
mDistId(0)
{
}


AosBitmapTrans::AosBitmapTrans(
		const AosTransType::E type,
		const u64 &docid,
		const bool need_save,
		const bool need_resp)
:
AosCubicTrans(type, AosXmlDoc::getOwnDocid(docid), need_save, need_resp),
mDistId(docid)
{
}


AosBitmapTrans::AosBitmapTrans(
		const AosTransType::E type,
		const u32 vid,
		const bool is_cube,
		const bool need_save,
		const bool need_resp)
:
AosCubicTrans(type, vid, is_cube, need_save, need_resp),
mDistId(vid)
{
}


AosBitmapTrans::~AosBitmapTrans()
{
}


bool
AosBitmapTrans::serializeTo(const AosBuffPtr &buff)
{
	AosCubicTrans::serializeTo(buff);
	return true;
}


bool
AosBitmapTrans::serializeFrom(const AosBuffPtr &buff)
{
	AosCubicTrans::serializeFrom(buff);
	return true;
}


bool	
AosBitmapTrans::directProc()
{
	u32 idx = mDistId % smProcThrd->getNumThrds();
	AosTransPtr thisptr(this, false);
	return smProcThrd->addRequest(idx, thisptr);	
}

