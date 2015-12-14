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
// 10/28/2013	Created by Young
////////////////////////////////////////////////////////////////////////////
#include "DocTrans/BatchAddSmallDocTrans.h"

#include "API/AosApi.h"
#include "SEUtil/Docid.h"
#include "SEInterfaces/DocClientObj.h"
#include "SeLogSvr/PhyLogSvr.h"
#include "XmlUtil/SeXmlParser.h"
#include "SEInterfaces/SmallDocStoreObj.h"


AosBatchAddSmallDocTrans::AosBatchAddSmallDocTrans(const bool regflag)
:
AosDocTrans(AosTransType::eBatchAddSmallDoc, regflag)
{
}


AosBatchAddSmallDocTrans::AosBatchAddSmallDocTrans(
		const u64 &docid,
		const u32 &logEntryNums,
		const AosBuffPtr &contents,
		const AosRundataPtr &rdata)
:
AosDocTrans(AosTransType::eBatchAddSmallDoc, docid, false, false, 0), 
mGroupId(docid),
mLogEntryNums(logEntryNums),
mEntrysBuff(contents)
{
}


AosBatchAddSmallDocTrans::~AosBatchAddSmallDocTrans()
{
}


bool
AosBatchAddSmallDocTrans::serializeFrom(const AosBuffPtr &buff)
{
	bool rslt = AosDocTrans::serializeFrom(buff);
	aos_assert_r(rslt, false);
	
	mGroupId = buff->getU64(0);
	mLogEntryNums = buff->getU32(0);
	aos_assert_r(mLogEntryNums != 0, false);
	u32 len = buff->getU32(0);
	mEntrysBuff = buff->getBuff(len, true AosMemoryCheckerArgs);
	return true;
}


bool
AosBatchAddSmallDocTrans::serializeTo(const AosBuffPtr &buff)
{
	bool rslt = AosDocTrans::serializeTo(buff);
	aos_assert_r(rslt, false);
	
	aos_assert_r(mEntrysBuff, false);
	buff->setU64(mGroupId);
	buff->setU32(mLogEntryNums);
	buff->setU32(mEntrysBuff->dataLen());
	buff->addBuff(mEntrysBuff);
	return true;
}


AosTransPtr
AosBatchAddSmallDocTrans::clone()
{
	return OmnNew AosBatchAddSmallDocTrans(false);
}


bool
AosBatchAddSmallDocTrans::proc()
{
	AosTransId trans_id = getTransId();
	bool rslt = AosSmallDocStoreObj::getSmallDocStoreObj()->addDocs(mGroupId, mLogEntryNums, mEntrysBuff, trans_id, mRdata);
	aos_assert_r(rslt, false);
	return true;
}


