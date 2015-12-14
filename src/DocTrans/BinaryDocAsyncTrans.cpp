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
// 05/17/2013	Created by Young Pan 
////////////////////////////////////////////////////////////////////////////
#include "DocTrans/BinaryDocAsyncTrans.h"

#include "XmlUtil/XmlTag.h"
#include "SEInterfaces/DocSvrObj.h"
#include "DataScanner/BinaryDocScanner.h"

AosBinaryDocAsyncTrans::AosBinaryDocAsyncTrans(const bool regflag)
:
AosDocTrans(AosTransType::eBinaryDoc, regflag)
{
}


AosBinaryDocAsyncTrans::AosBinaryDocAsyncTrans(
		const AosAsyncRespCallerPtr &resp_caller,
		const u64 &binaryDocId,
		const bool need_save,	
		const bool need_resp,
		const u64 snap_id)
:
AosDocTrans(AosTransType::eBinaryDoc, 
		binaryDocId, need_save, need_resp, snap_id),
mBinaryDocId(binaryDocId),
mRespCaller(resp_caller)
{
}


AosBinaryDocAsyncTrans::~AosBinaryDocAsyncTrans()
{
}


bool
AosBinaryDocAsyncTrans::serializeFrom(const AosBuffPtr &buff)
{
	bool rslt = AosDocTrans::serializeFrom(buff);
	aos_assert_r(rslt, false);

	mBinaryDocId = buff->getU64(0);
	aos_assert_r(mBinaryDocId != 0, false);

	setDistId(mBinaryDocId);

	return true;
}


bool
AosBinaryDocAsyncTrans::serializeTo(const AosBuffPtr &buff)
{
	bool rslt = AosDocTrans::serializeTo(buff);
	aos_assert_r(rslt, false);

	buff->setU64(mBinaryDocId);
	return true;
}


AosTransPtr
AosBinaryDocAsyncTrans::clone()
{
	return OmnNew AosBinaryDocAsyncTrans(false);
}


bool
AosBinaryDocAsyncTrans::proc()
{
OmnScreen << " ================ proc : " << mBinaryDocId << endl;
	AosBuffPtr buff = getDoc();
	AosBuffPtr resp_buff;
	if(!buff)
	{
		resp_buff = OmnNew AosBuff(10 AosMemoryCheckerArgs);
		resp_buff->setU8(false);
	}
	else
	{
		u32 doc_len = buff->dataLen();
		resp_buff = OmnNew AosBuff(doc_len + 10 AosMemoryCheckerArgs);
		resp_buff->setU8(true);
		resp_buff->setU32(doc_len);
		resp_buff->setBuff((char *)buff->data(), doc_len);
	}
	sendResp(resp_buff);

	return true;
}

AosBuffPtr
AosBinaryDocAsyncTrans::getDoc()
{
	AosXmlTagPtr doc = AosDocSvrObj::getDocSvr()->getDoc(mBinaryDocId, mSnapshotId, mRdata);
	if(!doc) return NULL;

	aos_assert_r(doc->getAttrU64(AOSTAG_DOCID, 0) == mBinaryDocId, NULL);
	aos_assert_r(doc->getAttrStr(AOSTAG_OTYPE) == AOSOTYPE_BINARYDOC, NULL);
	//aos_assert_r(doc->getAttrStr(AOSTAG_BINARY_DOC_SIGNATURE) != "", false);

	doc = doc->clone(AosMemoryCheckerArgsBegin);
	aos_assert_r(doc, NULL);

	AosBuffPtr buff;
	bool rslt = AosDocSvrObj::getDocSvr()->retrieveBinaryDoc(doc, buff, mRdata, mSnapshotId);
	if (!rslt) return NULL;

	aos_assert_r(buff && buff->dataLen() > 0, NULL);

	OmnString nodename = doc->getAttrStr(AOSTAG_BINARY_NODENAME);
	doc->setTextBinary(nodename, buff);
	//doc->removeAttr(AOSTAG_BINARY_DOC_SIGNATURE);

	OmnString docstr = doc->toString();
	AosBuffPtr docbuff = OmnNew AosBuff(docstr.length() AosMemoryCheckerArgs);
	aos_assert_r(docbuff, NULL);

	docbuff->setBuff(docstr.data(), docstr.length());
	return docbuff;
}


bool
AosBinaryDocAsyncTrans::respCallBack()
{
OmnScreen << " ================ proc : " << mBinaryDocId << endl;
	// Ketty 2013/07/20
	AosBuffPtr resp = getResp();
	bool svr_death = isSvrDeath();
	
	// will call BinaryDocScanner::callback();
	// will call BinaryDocScanner::binaryDocTransCallback();
	AosTransPtr thisptr(this, false);
	mRespCaller->callback(thisptr, resp, svr_death);
	return true;
}


