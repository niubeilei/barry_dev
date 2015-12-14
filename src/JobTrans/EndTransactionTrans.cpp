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
#include "JobTrans/EndTransactionTrans.h"

#include "API/AosApi.h"
#include "XmlUtil/XmlTag.h"
#include "SEInterfaces/JobMgrObj.h"

AosEndTransactionTrans::AosEndTransactionTrans(const bool regflag)
:
AosTaskTrans(AosTransType::eEndTransaction, regflag)
{
}


AosEndTransactionTrans::AosEndTransactionTrans(
		const int server_id,
		const u64 &job_docid)
:
AosTaskTrans(AosTransType::eEndTransaction, server_id, true, false),
mJobDocid(job_docid)
{
}


AosEndTransactionTrans::~AosEndTransactionTrans()
{
}


bool
AosEndTransactionTrans::serializeFrom(const AosBuffPtr &buff)
{
	bool rslt = AosTaskTrans::serializeFrom(buff);
	aos_assert_r(rslt, false);

	mJobDocid = buff->getU64(0);

	return true;
}


bool
AosEndTransactionTrans::serializeTo(const AosBuffPtr &buff)
{
	bool rslt = AosTaskTrans::serializeTo(buff);
	aos_assert_r(rslt, false);

	buff->setU64(mJobDocid);

	return true;
}


AosTransPtr
AosEndTransactionTrans::clone()
{
	return OmnNew AosEndTransactionTrans(false);
}


bool
AosEndTransactionTrans::proc()
{
	/*
	bool rslt = AosJobMgrObj::endTransactionLocalStatic(mJobDocid, mRdata);

	AosBuffPtr resp_buff = OmnNew AosBuff(10 AosMemoryCheckerArgs);
	resp_buff->setU8(rslt);

	// Ketty 2013/07/23
	sendResp(resp_buff);
	//AosTransPtr thisptr(this, false);
	//AosSendResp(thisptr, resp_buff);
	//OmnString str;
	//str << "<rsp rslt=\"" << rslt << "\"/>";
	//resp_buff->setBuff(str.data(), str.length());
	*/
	return true;
}


