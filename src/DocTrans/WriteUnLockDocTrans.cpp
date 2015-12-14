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
#include "DocTrans/WriteUnLockDocTrans.h"

#include "API/AosApi.h"
#include "SEInterfaces/DocSvrObj.h"
#include "XmlUtil/XmlTag.h"

AosWriteUnLockTrans::AosWriteUnLockTrans(const bool regflag)
:
AosDocTrans(AosTransType::eWriteUnLockDoc, regflag)
{
}


AosWriteUnLockTrans::AosWriteUnLockTrans(
		const u64 &docid,
		const u64 &userid,
		const AosXmlTagPtr &newdoc,
		const bool need_save,
		const bool need_resp,
		const u64 snap_id)
:
AosDocTrans(AosTransType::eWriteUnLockDoc, docid, need_save, need_resp, snap_id),
mDocid(docid),
mUserid(userid),
mNewDoc(newdoc)
{
}


AosWriteUnLockTrans::~AosWriteUnLockTrans()
{
}


bool
AosWriteUnLockTrans::serializeFrom(const AosBuffPtr &buff)
{
	bool rslt = AosDocTrans::serializeFrom(buff);
	aos_assert_r(rslt, false);
	
	mDocid = buff->getU64(0);
	mUserid = buff->getU64(0);

	u32 data_len = buff->getU32(0);
	AosBuffPtr doc_buff = buff->getBuff(data_len, true AosMemoryCheckerArgs);
	
	AosXmlParser parser;
	mNewDoc= parser.parse(doc_buff->data(), data_len, "" AosMemoryCheckerArgs);

	aos_assert_r(mDocid && mNewDoc, false);
	setDistId(mDocid);
	return true;
}


bool
AosWriteUnLockTrans::serializeTo(const AosBuffPtr &buff)
{
	bool rslt = AosDocTrans::serializeTo(buff);
	aos_assert_r(rslt, false);
	
	buff->setU64(mDocid);
	buff->setU64(mUserid);
	
	u32 data_len = mNewDoc->getDataLength();
	buff->setU32(data_len);
	buff->setBuff((char *)mNewDoc->getData(), data_len);
	return true;
}


AosTransPtr
AosWriteUnLockTrans::clone()
{
	return OmnNew AosWriteUnLockTrans(false);
}


bool
AosWriteUnLockTrans::proc()
{
	bool rslt = AosDocSvrObj::getDocSvr()->writeUnLockDoc(mDocid, mUserid, mNewDoc, getTransId(), mRdata.getPtr());

	AosBuffPtr resp_buff = OmnNew AosBuff(100 AosMemoryCheckerArgs);
	resp_buff->setU8(rslt);
	resp_buff->setOmnStr(mRdata->getErrmsg());
	
	sendResp(resp_buff);
	return true;
}

bool
AosWriteUnLockTrans::procGetResp()
{
	// Ketty 2013/09/16
	return proc();
}

