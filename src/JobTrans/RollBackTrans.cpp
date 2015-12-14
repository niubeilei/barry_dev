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
#include "JobTrans/RollBackTrans.h"

#include "API/AosApi.h"
#include "XmlUtil/XmlTag.h"
#include "SEInterfaces/JobMgrObj.h"

AosRollBackTrans::AosRollBackTrans(const bool regflag)
:
AosTaskTrans(AosTransType::eRollBack, regflag)
{
}


AosRollBackTrans::AosRollBackTrans(
		const int svr_id)
:
AosTaskTrans(AosTransType::eRollBack, svr_id, true, true)
{
}


AosRollBackTrans::~AosRollBackTrans()
{
}


bool
AosRollBackTrans::serializeFrom(const AosBuffPtr &buff)
{
	bool rslt = AosTaskTrans::serializeFrom(buff);
	aos_assert_r(rslt, false);
	return true;
}


bool
AosRollBackTrans::serializeTo(const AosBuffPtr &buff)
{
	bool rslt = AosTaskTrans::serializeTo(buff);
	aos_assert_r(rslt, false);
	return true;
}


AosTransPtr
AosRollBackTrans::clone()
{
	return OmnNew AosRollBackTrans(false);
}


bool
AosRollBackTrans::proc()
{
	bool rslt = false; //AosJobMgrObj::rollBackLocalStatic(mRdata);

	AosBuffPtr resp_buff = OmnNew AosBuff(10 AosMemoryCheckerArgs);
	resp_buff->setU8(rslt);

	// Ketty 2013/07/23
	sendResp(resp_buff);
	//AosTransPtr thisptr(this, false);
	//AosSendResp(thisptr, resp_buff);
	//OmnString str;
	//str << "<rsp rslt=\"" << rslt << "\"/>";
	//resp_buff->setBuff(str.data(), str.length());
	return true;
}


