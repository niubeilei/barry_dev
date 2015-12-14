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
#include "DocTrans/GetDocTesterTrans.h"

#include "API/AosApi.h"
#include "DocServer/DocSvr.h"
#include "XmlUtil/XmlTag.h"

AosGetDocTesterTrans::AosGetDocTesterTrans(const bool regflag)
:
AosDocTrans(AosTransType::eGetDocTester, regflag)
{
}


AosGetDocTesterTrans::AosGetDocTesterTrans(
		const u64 docid,
		const bool need_save,
		const bool need_resp,
		const u64 snap_id)
:
AosDocTrans(AosTransType::eGetDocTester, docid, false, need_resp, snap_id),
mDocid(docid)
{
}


AosGetDocTesterTrans::~AosGetDocTesterTrans()
{
}


bool
AosGetDocTesterTrans::serializeFrom(const AosBuffPtr &buff)
{
	bool rslt = AosDocTrans::serializeFrom(buff);
	aos_assert_r(rslt, false);
	
	mDocid = buff->getU64(0);
	aos_assert_r(mDocid != 0, false);
	setDistId(mDocid);
	return true;
}


bool
AosGetDocTesterTrans::serializeTo(const AosBuffPtr &buff)
{
	bool rslt = AosDocTrans::serializeTo(buff);
	aos_assert_r(rslt, false);
	
	buff->setU64(mDocid);
	return true;
}


AosTransPtr
AosGetDocTesterTrans::clone()
{
	return OmnNew AosGetDocTesterTrans(false);
}


bool
AosGetDocTesterTrans::proc()
{
	AosBuffPtr doc_buff = 0;// AosDocSvr::getSelf()->getDocTester(mDocid, mRdata);	
	AosBuffPtr resp_buff;
	if(doc_buff->dataLen() <= 0)
	{
		resp_buff = OmnNew AosBuff(10 AosMemoryCheckerArgs);
		resp_buff->setU8(false);
	}
	else
	{
		u32 doc_len = doc_buff->dataLen();
		resp_buff = OmnNew AosBuff(doc_len + 10 AosMemoryCheckerArgs);
		resp_buff->setU8(true);
		resp_buff->setU32(doc_len);
		resp_buff->setBuff(doc_buff->data(), doc_len);
	}
	
	sendResp(resp_buff);
	return true;
}
	

bool
AosGetDocTesterTrans::procGetResp()
{
	AosBuffPtr resp_buff = OmnNew AosBuff(10 AosMemoryCheckerArgs);
	resp_buff->setU8(3);
	sendResp(resp_buff);
	return true;
}


