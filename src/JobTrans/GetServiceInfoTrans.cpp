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
// 2015/08/21	Created by Jozhi
////////////////////////////////////////////////////////////////////////////
#include "JobTrans/GetServiceInfoTrans.h"

#include "API/AosApi.h"
#include "XmlUtil/XmlTag.h"
#include "StreamEngine/Ptrs.h"
#include "StreamEngine/Service.h" 
#include "StreamEngine/StreamDataProc.h"
#include "StreamEngine/ServiceMgr.h"

AosGetServiceInfoTrans::AosGetServiceInfoTrans(const bool regflag)
:
AosTaskTrans(AosTransType::eGetServiceInfo, regflag)
{
}


AosGetServiceInfoTrans::AosGetServiceInfoTrans(
		const int server_id,
		const u32 to_proc_id)
:
AosTaskTrans(AosTransType::eGetServiceInfo, server_id, to_proc_id, false, true)
{
}


AosGetServiceInfoTrans::~AosGetServiceInfoTrans()
{
}


bool
AosGetServiceInfoTrans::serializeFrom(const AosBuffPtr &buff)
{
	bool rslt = AosTaskTrans::serializeFrom(buff);
	aos_assert_r(rslt, false);
	return true;
}


bool
AosGetServiceInfoTrans::serializeTo(const AosBuffPtr &buff)
{
	bool rslt = AosTaskTrans::serializeTo(buff);
	aos_assert_r(rslt, false);
	return true;
}


AosTransPtr
AosGetServiceInfoTrans::clone()
{
	return OmnNew AosGetServiceInfoTrans(false);
}


bool
AosGetServiceInfoTrans::proc()
{
	//1. get the service info from local sgServiceMap[xxxx];
	//2. set the info msg to the resp_buff
	
	OmnString msg = ""; 
	msg = AosServiceMgr::getSelf()->getServiceInfo(mRdata.getPtr());

	//send resp_buff to JqlService
	AosBuffPtr resp_buff = OmnNew AosBuff(AosMemoryCheckerArgsBegin);
	aos_assert_r(resp_buff,false);

	resp_buff->setU8(resp_buff);
	resp_buff->setOmnStr(msg);
	sendResp(resp_buff);
	mRdata->setOk();
    return true;
}

