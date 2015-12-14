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
//Linda, 2013/06/09 Moved to DocTrans/BinaryDocAsyncTrans.h
#if 0
#include "JobTrans/BinaryDocTrans.h"

#include "XmlUtil/XmlTag.h"
#include "DocServer/DocSvr.h"
#include "SEInterfaces/NetFileMgrObj.h"

AosBinaryDocTrans::AosBinaryDocTrans(const bool regflag)
:
AosAsyncReqTrans(AosTransType::eBinaryDoc, regflag)
{

}


AosBinaryDocTrans::AosBinaryDocTrans(
		const u64 &binaryDocId,
		const int svr_id,
		const bool need_save,	
		const bool need_resp)
:
AosAsyncReqTrans(AosTransType::eBinaryDoc, svr_id, need_save, need_resp)
{
	mBinaryDocId =  binaryDocId;
}


AosBinaryDocTrans::~AosBinaryDocTrans()
{
}


bool
AosBinaryDocTrans::serializeFrom(const AosBuffPtr &buff)
{
	bool rslt = AosAsyncReqTrans::serializeFrom(buff);
	aos_assert_r(rslt, false);

	/*
	mFileName = buff->getOmnStr("");
	mSeekPos = buff->getInt64(0);
	//int crtIdx = buff->getCrtIdx();
	//mBuff = OmnNew AosBuff(&(buff->data())[crtIdx], buff_len, buff_len, 0 AosMemoryCheckerArgs);
	*/

	mBinaryDocId = buff->getU64(0);
	aos_assert_r(mBinaryDocId != 0, false);
	return true;
}


bool
AosBinaryDocTrans::serializeTo(const AosBuffPtr &buff)
{
	bool rslt = AosAsyncReqTrans::serializeTo(buff);
	aos_assert_r(rslt, false);

	buff->setU64(mBinaryDocId);

	//buff->setOmnStr(mBinaryDocId);
	//buff->setI64(mSeekPos);
	//buff->setU32(mBuff->dataLen());
	//buff->setBuff(buff);
	
	return true;
}


AosTransPtr
AosBinaryDocTrans::clone()
{
	return OmnNew AosBinaryDocTrans(false);
}


bool
AosBinaryDocTrans::proc()
{
//	AosBuffPtr tmpbuff;
//	AosXmlTagPtr tmpdoc = AosRetrieveBinaryDoc(mBinaryDocId, tmpbuff, mRdata);
//	aos_assert_r(tmpdoc, false);
//
//	aos_assert_r(tmpbuff->dataLen() > 0, false);
//	OmnString nodename = tmpdoc->getAttrStr(AOSTAG_BINARY_NODENAME);
//	tmpdoc->setTextBinary(nodename, tmpbuff);
//
//	OmnString docstr = tmpdoc->toString();
//	AosBuffPtr docbuff = OmnNew AosBuff(docstr.getBuffer(), docstr.length(), docstr.length(), 0 AosMemoryCheckerArgs);
//	aos_assert_r(docbuff, false);
//
//	//sendResp(tmpbuff);
	AosXmlTagPtr tmpdoc = AosDocSvr::getSelf()->getDoc(mBinaryDocId, mRdata);
	if(!tmpdoc)    return 0;

	aos_assert_r(tmpdoc->getAttrU64(AOSTAG_DOCID, 0) == mBinaryDocId, 0);

	//Linda 2012/08/21 get binary doc
	if (tmpdoc->getAttrStr(AOSTAG_OTYPE, "") == AOSOTYPE_BINARYDOC)
	{
		tmpdoc = tmpdoc->clone(AosMemoryCheckerArgsBegin);
		aos_assert_r(tmpdoc, 0);
	}

	//aos_assert_r(tmpdoc->getAttrStr(AOSTAG_BINARY_DOC_SIGNATURE, "") != "", 0);
	aos_assert_r(tmpdoc->getAttrStr(AOSTAG_OTYPE, "") == AOSOTYPE_BINARYDOC, 0);
	AosBuffPtr buff;
	bool rslt = AosDocSvr::getSelf()->retrieveBinaryDoc(tmpdoc, buff, mRdata);
	if (!rslt)  return 0;
	aos_assert_r(buff->dataLen() > 0, tmpdoc);
	OmnString nodename = tmpdoc->getAttrStr(AOSTAG_BINARY_NODENAME);
	tmpdoc->setTextBinary(nodename, buff);
	//tmpdoc->removeAttr(AOSTAG_BINARY_DOC_SIGNATURE);

	OmnString docstr = tmpdoc->toString();
	AosBuffPtr docbuff = OmnNew AosBuff(docstr.getBuffer(), docstr.length(), docstr.length(), 0 AosMemoryCheckerArgs);
	aos_assert_r(docbuff, false);
	sendResp(docbuff);

	return true;
}

#endif
