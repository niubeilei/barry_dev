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
#include "LogTrans/RetrieveLogTrans.h"

#include "API/AosApi.h"
#include "SeLogSvr/PhyLogSvr.h"

AosRetrieveLogTrans::AosRetrieveLogTrans(const bool regflag)
:
AosCubicTrans(AosTransType::eRetrieveLog, regflag)
{
}


AosRetrieveLogTrans::AosRetrieveLogTrans(
		const u64 logid,
		const bool need_save,
		const bool need_resp)
:
AosCubicTrans(AosTransType::eRetrieveLog, logid, need_save, need_resp),
mLogId(logid)
{
}


AosRetrieveLogTrans::~AosRetrieveLogTrans()
{
}


bool
AosRetrieveLogTrans::serializeFrom(const AosBuffPtr &buff)
{
	bool rslt = AosCubicTrans::serializeFrom(buff);
	aos_assert_r(rslt, false);

	mLogId = buff->getU64(0);
	aos_assert_r(mLogId, false);
	return true;
}


bool
AosRetrieveLogTrans::serializeTo(const AosBuffPtr &buff)
{
	bool rslt = AosCubicTrans::serializeTo(buff);
	aos_assert_r(rslt, false);
	
	buff->setU64(mLogId);
	return true;
}


AosTransPtr
AosRetrieveLogTrans::clone()
{
	return OmnNew AosRetrieveLogTrans(false);
}


bool
AosRetrieveLogTrans::proc()
{
	AosXmlTagPtr log = AosPhyLogSvr::getSelf()->retrieveLog(mLogId, mRdata);
	aos_assert_r(log, false);

	u32 log_len = log->getDataLength();
	
	AosBuffPtr resp_buff = OmnNew AosBuff(log_len + 10 AosMemoryCheckerArgs);
	resp_buff->setU8(true);
	resp_buff->setU32(log_len);
	resp_buff->setBuff((char *)log->getData(), log_len);
	
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


