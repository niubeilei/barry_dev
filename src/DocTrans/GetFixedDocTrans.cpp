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
#include "DocTrans/GetFixedDocTrans.h"

#include "API/AosApi.h"
#include "SEInterfaces/DocSvrObj.h"
#include "XmlUtil/XmlTag.h"


AosGetFixedDocTrans::AosGetFixedDocTrans(const bool regflag)
:
AosDocTrans(AosTransType::eGetFixedDoc, regflag)
{
}


AosGetFixedDocTrans::AosGetFixedDocTrans(
		const u64 &docid,
		const int record_len,
		const bool need_save,
		const bool need_resp,
		const u64 snap_id)
:
AosDocTrans(AosTransType::eGetFixedDoc, docid, need_save, need_resp, snap_id),
mDocid(docid),
mRecordLen(record_len)
{
}


AosGetFixedDocTrans::~AosGetFixedDocTrans()
{
}


bool
AosGetFixedDocTrans::serializeFrom(const AosBuffPtr &buff)
{
	bool rslt = AosDocTrans::serializeFrom(buff);
	aos_assert_r(rslt, false);
	
	mDocid = buff->getU64(0);
	mRecordLen = buff->getInt(-1);

	aos_assert_r(mDocid != 0, false);
	aos_assert_r(mRecordLen > 0, false);
	setDistId(mDocid);
	return true;
}


bool
AosGetFixedDocTrans::serializeTo(const AosBuffPtr &buff)
{
	bool rslt = AosDocTrans::serializeTo(buff);
	aos_assert_r(rslt, false);

	buff->setU64(mDocid);
	buff->setInt(mRecordLen);
	return true;
}


AosTransPtr
AosGetFixedDocTrans::clone()
{
	return OmnNew AosGetFixedDocTrans(false);
}


bool
AosGetFixedDocTrans::proc()
{
	AosXmlTagPtr doc = getDoc();	

	AosBuffPtr resp_buff;
	if(!doc)
	{
		resp_buff = OmnNew AosBuff(10 AosMemoryCheckerArgs);
		resp_buff->setU8(false);
	}
	else
	{
		u32 doc_len = doc->getDataLength();
		resp_buff = OmnNew AosBuff(doc_len + 10 AosMemoryCheckerArgs);
		resp_buff->setU8(true);
		resp_buff->setU32(doc_len);
		resp_buff->setBuff((char *)doc->getData(), doc_len);
	}
	
	// Ketty 2013/07/23
	sendResp(resp_buff);
	//AosTransPtr thisptr(this, false);
	//AosSendResp(thisptr, resp_buff);
	return true;
}


AosXmlTagPtr
AosGetFixedDocTrans::getDoc()
{
	AosXmlTagPtr doc = AosDocSvrObj::getDocSvr()->getFixedDoc(mDocid, mRecordLen, mSnapshotId, mRdata);	
	if(!doc)	return 0;

	aos_assert_r(doc->getAttrU64(AOSTAG_DOCID, 0) == mDocid, 0);
	return doc;
}


