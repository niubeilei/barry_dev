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
// 07/28/2011	Created by Linda 
////////////////////////////////////////////////////////////////////////////
#include "DocTrans/GetSmallDocTrans.h"

#include "API/AosApi.h"
//#include "DocServer/DocSvr.h"
#include "SEInterfaces/SmallDocStoreObj.h"
#include "XmlUtil/XmlTag.h"


AosGetSmallDocTrans::AosGetSmallDocTrans(const bool regflag)
:
AosDocTrans(AosTransType::eGetSmallDoc, regflag)
{
}


AosGetSmallDocTrans::AosGetSmallDocTrans(
		const u64 docid,
		const u32 entryNums,
		const bool need_save,
		const bool need_resp)
:
AosDocTrans(AosTransType::eGetSmallDoc, docid, false, need_resp, 0),
mDocid(docid), 
mMaxEntryNums(entryNums)
{
}


AosGetSmallDocTrans::~AosGetSmallDocTrans()
{
}


bool
AosGetSmallDocTrans::serializeFrom(const AosBuffPtr &buff)
{
	bool rslt = AosDocTrans::serializeFrom(buff);
	aos_assert_r(rslt, false);
	
	mDocid = buff->getU64(0);
	mMaxEntryNums = buff->getU32(0);
	aos_assert_r(mDocid > 0 && mMaxEntryNums > 0, false);
	setDistId(mDocid);
	return true;
}


bool
AosGetSmallDocTrans::serializeTo(const AosBuffPtr &buff)
{
	bool rslt = AosDocTrans::serializeTo(buff);
	aos_assert_r(rslt, false);
	
	buff->setU64(mDocid);
	buff->setU32(mMaxEntryNums);
	return true;
}


AosTransPtr
AosGetSmallDocTrans::clone()
{
	return OmnNew AosGetSmallDocTrans(false);
}


bool
AosGetSmallDocTrans::proc()
{
	AosBuffPtr buff = AosSmallDocStoreObj::getSmallDocStoreObj()->retrieveDoc(mRdata, mDocid, mMaxEntryNums);
	aos_assert_r(buff, false);

	u32 doc_len = buff->dataLen();
	aos_assert_r(doc_len > 0, false);

	AosBuffPtr resp_buff = OmnNew AosBuff(doc_len + 8 AosMemoryCheckerArgs);
	resp_buff->setU8(true);
	resp_buff->setU32(doc_len);
	resp_buff->addBuff(buff);
	sendResp(resp_buff);
	return true;
}


