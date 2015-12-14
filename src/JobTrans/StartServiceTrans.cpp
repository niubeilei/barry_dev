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
// 2015/08/13	Created by Jozhi
////////////////////////////////////////////////////////////////////////////
#include "JobTrans/StartServiceTrans.h"

#include "API/AosApi.h"
#include "XmlUtil/XmlTag.h"
#include "SEInterfaces/JobMgrObj.h"
#include "Job/JobMgr.h"
#include "TaskUtil/TaskDriver.h"
#include "TaskUtil/TaskRunnerInfo.h"
#include "TaskUtil/TaskDriverRequest.h"
#include "SEInterfaces/TaskMgrObj.h"
#include "StreamEngine/Service.h"


AosStartServiceTrans::AosStartServiceTrans(const bool regflag)
:
AosTaskTrans(AosTransType::eStartService, regflag)
{
}


AosStartServiceTrans::AosStartServiceTrans(
		const u64 &service_docid,
		const int server_id,
		const u32 to_proc_id)
:
AosTaskTrans(AosTransType::eStartService, server_id, to_proc_id,  false, false),
mServiceDocid(service_docid)
{
}


AosStartServiceTrans::~AosStartServiceTrans()
{
}


bool
AosStartServiceTrans::serializeFrom(const AosBuffPtr &buff)
{
	bool rslt = AosTaskTrans::serializeFrom(buff);
	aos_assert_r(rslt, false);

	mServiceDocid = buff->getU64(0);
	aos_assert_r(mServiceDocid != 0, false);
	return true;
}


bool
AosStartServiceTrans::serializeTo(const AosBuffPtr &buff)
{
	bool rslt = AosTaskTrans::serializeTo(buff);
	aos_assert_r(rslt, false);

	aos_assert_r(mServiceDocid != 0, false);
	buff->setU64(mServiceDocid);
	return true;
}


AosTransPtr
AosStartServiceTrans::clone()
{
	return OmnNew AosStartServiceTrans(false);
}


bool
AosStartServiceTrans::proc()
{
	//1. get the service doc, create the service instance
	//2. start the service
	AosServicePtr service = OmnNew AosService();
	aos_assert_r(service, false);
	bool rslt = service->runService(mRdata.getPtr(),mServiceDocid);
	aos_assert_r(rslt, false);
	return true;
}

