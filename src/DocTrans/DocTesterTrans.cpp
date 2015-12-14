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
#include "DocTrans/DocTesterTrans.h"

#include "SEInterfaces/DocSvrObj.h"
#include "XmlUtil/SeXmlParser.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/XmlDoc.h"

AosDocTesterTrans::AosDocTesterTrans(const bool regflag)
:
AosDocTrans(AosTransType::eDocTester, regflag)
{
}


AosDocTesterTrans::AosDocTesterTrans(
		const int opr,
		const u64 docid,
		const char *new_doc,
		const int data_len,   
		const bool need_save,
		const bool need_resp,
		const u64 snap_id)
:
AosDocTrans(AosTransType::eDocTester,
		docid, need_save, need_resp, snap_id),
mOpr(opr),
mDocid(docid),
mDocBuff(0),
mDocLen(data_len)
{
	if (data_len > 0)
	{
		mDocBuff = OmnNew AosBuff(mDocLen AosMemoryCheckerArgs);
		mDocBuff->setBuff(new_doc, mDocLen);
	}
}


AosDocTesterTrans::~AosDocTesterTrans()
{
}


bool
AosDocTesterTrans::serializeFrom(const AosBuffPtr &buff)
{
	bool rslt = AosDocTrans::serializeFrom(buff);
	aos_assert_r(rslt, false);
	
	mOpr = buff->getInt(-1);
	mDocid = buff->getU64(0);
	mDocLen = buff->getU32(0);
	if (mDocLen > 0)
	{
		mDocBuff = buff->getBuff(mDocLen, true AosMemoryCheckerArgs);
		aos_assert_r(mDocid && mDocLen, false);
	}
	setDistId(mDocid);
	return true;
}


bool
AosDocTesterTrans::serializeTo(const AosBuffPtr &buff)
{
	bool rslt = AosDocTrans::serializeTo(buff);
	aos_assert_r(rslt, false);
	
	buff->setInt(mOpr);
	buff->setU64(mDocid);
	buff->setU32(mDocLen);
	if (mDocLen > 0)
	{
		buff->setBuff(mDocBuff);
		aos_assert_r(mDocBuff && mDocLen > 0, false);
	}
	return true;
}


AosTransPtr
AosDocTesterTrans::clone()
{
	return OmnNew AosDocTesterTrans(false);
}


bool
AosDocTesterTrans::proc()
{
	setFinishLater();

	bool rslt = false;
	if (mOpr == 1)
	{
		// create and modify
		aos_assert_r(mDocBuff && mDocLen > 0, false);
		rslt = AosDocSvrObj::getDocSvr()->createDoc(mRdata, mDocid, mDocBuff->data(), mDocLen, getTransId(), mSnapshotId);
	}

	if (mOpr == 2)
	{
		//deletedoc
		rslt = AosDocSvrObj::getDocSvr()->deleteObj(mRdata, mDocid, getTransId(), mSnapshotId);
	}
	AosBuffPtr resp_buff = OmnNew AosBuff(10 AosMemoryCheckerArgs);
	resp_buff->setU8(rslt);
	sendResp(resp_buff);
	return true;
}

OmnString
AosDocTesterTrans::getStrType()
{
	OmnString str = "eTester__";
	str << (mOpr == 1 ? "save" : "delete");
	return str;
}

