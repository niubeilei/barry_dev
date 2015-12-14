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
#include "JobTrans/NotifyJobTaskStopTrans.h"

#include "API/AosApi.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/SeXmlParser.h"
#include "TaskUtil/Ptrs.h"
#include "TaskUtil/TaskDriver.h"
#include "TaskUtil/TaskRunnerInfo.h"
#include "TaskUtil/TaskDriverRequest.h"


AosNotifyJobTaskStopTrans::AosNotifyJobTaskStopTrans(const bool regflag)
:
AosTaskTrans(AosTransType::eNotifyJobTaskStop, regflag)
{
}


AosNotifyJobTaskStopTrans::AosNotifyJobTaskStopTrans(
		const int job_server_id,
		const int task_serverid)
:
AosTaskTrans(AosTransType::eNotifyJobTaskStop, job_server_id, false, false),
mTaskServerId(task_serverid)
{
}


AosNotifyJobTaskStopTrans::~AosNotifyJobTaskStopTrans()
{
}


bool
AosNotifyJobTaskStopTrans::serializeFrom(const AosBuffPtr &buff)
{
	bool rslt = AosTaskTrans::serializeFrom(buff);
	aos_assert_r(rslt, false);

	mTaskServerId = buff->getInt(0);
	return true;
}


bool
AosNotifyJobTaskStopTrans::serializeTo(const AosBuffPtr &buff)
{
	bool rslt = AosTaskTrans::serializeTo(buff);
	aos_assert_r(rslt, false);

	buff->setInt(mTaskServerId);
	return true;
}


AosTransPtr
AosNotifyJobTaskStopTrans::clone()
{
	return OmnNew AosNotifyJobTaskStopTrans(false);
}


bool
AosNotifyJobTaskStopTrans::proc()
{
	//OmnScreen << "AosNotifyJobTaskStopTrans AosNotifyJobTaskStopTrans task_server_id: " << mTaskServerId << endl;
	AosTaskDriverRequestPtr req = OmnNew AosTaskDriverRequest(AosTaskDriver::eNotifyJobTaskStop, mTaskServerId, mRdata);
	bool rslt = AosTaskDriver::getSelf()->addEvent(req);
	if (!rslt)
	{
		mRdata->setError();
		return true;
	}
	mRdata->setOk();
	return true;
}
