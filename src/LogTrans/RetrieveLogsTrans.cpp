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
#include "LogTrans/RetrieveLogsTrans.h"

#include "API/AosApi.h"
#include "SeLogSvr/PhyLogSvr.h"
#include "XmlUtil/SeXmlParser.h"

AosRetrieveLogsTrans::AosRetrieveLogsTrans(const bool regflag)
:
AosCubicTrans(AosTransType::eRetrieveLogs, regflag)
{
}


AosRetrieveLogsTrans::AosRetrieveLogsTrans(
		const AosXmlTagPtr &request,
		const u64 docid,
		const bool need_save,
		const bool need_resp)
:
AosCubicTrans(AosTransType::eRetrieveLogs, docid, need_save, need_resp),
mReq(request)
{
}


AosRetrieveLogsTrans::~AosRetrieveLogsTrans()
{
}


bool
AosRetrieveLogsTrans::serializeFrom(const AosBuffPtr &buff)
{
	bool rslt = AosCubicTrans::serializeFrom(buff);
	aos_assert_r(rslt, false);

	buff->setU32(mReq->getDataLength());
	buff->setBuff((char *)mReq->getData(), mReq->getDataLength());
	return true;
}


bool
AosRetrieveLogsTrans::serializeTo(const AosBuffPtr &buff)
{
	bool rslt = AosCubicTrans::serializeTo(buff);
	aos_assert_r(rslt, false);

	u32 req_len = buff->getU32(0);
	AosBuffPtr req_buff = buff->getBuff(req_len, false AosMemoryCheckerArgs);
	aos_assert_r(req_buff, false);

	AosXmlParser parser;
	mReq = parser.parse(req_buff->data(), req_len, "" AosMemoryCheckerArgs);
	return true;
}


AosTransPtr
AosRetrieveLogsTrans::clone()
{
	return OmnNew AosRetrieveLogsTrans(false);
}


bool
AosRetrieveLogsTrans::proc()
{
	OmnString resp_str = AosPhyLogSvr::getSelf()->retrieveLogs(mReq, mRdata);
	aos_assert_r(resp_str != "", false);
	
	AosBuffPtr resp_buff = OmnNew AosBuff(30 AosMemoryCheckerArgs);
	resp_buff->setU8(true);
	resp_buff->setOmnStr(resp_str);

	// Ketty 2013/07/23
	sendResp(resp_buff);
	//AosTransPtr thisptr(this, false);
	//AosSendResp(thisptr, resp_buff);
	
	//resp = "<Contents><record>";
	//resp << "<doc>" << doc->toString() << "</doc>"
	//	<< "</record></Contents>";
	//resp_buff = OmnNew AosBuff(doc->getDataLength() AosMemoryCheckerArgs);
	//resp_buff->setBuff((char *)doc->getData(), doc->getDataLength());
	return true;
}


