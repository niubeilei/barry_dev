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
// 2015/01/05	Created by Andy zhang
////////////////////////////////////////////////////////////////////////////
#include "JobTrans/SendClearIILEntryMapTrans.h"

#include "API/AosApi.h"
#include "DataCollector/DataCollectorMgr.h"
#include "XmlUtil/XmlTag.h"
#include "SEInterfaces/JobObj.h"
#include "SEInterfaces/JobMgrObj.h"
#include "SEInterfaces/NetFileMgrObj.h"
#include "TaskMgr/TaskMgr.h"


AosSendClearIILEntryMapTrans::AosSendClearIILEntryMapTrans(const bool regflag)
:
AosTaskTrans(AosTransType::eSendClearIILEntryMap, regflag)
{
}


AosSendClearIILEntryMapTrans::AosSendClearIILEntryMapTrans(
		const int svr_id)
:
AosTaskTrans(AosTransType::eSendClearIILEntryMap, svr_id, false, true)
{
}


AosSendClearIILEntryMapTrans::~AosSendClearIILEntryMapTrans()
{
}


bool
AosSendClearIILEntryMapTrans::serializeFrom(const AosBuffPtr &buff)
{
	bool rslt = AosTaskTrans::serializeFrom(buff);
	aos_assert_r(rslt, false);
	return true;
}


bool
AosSendClearIILEntryMapTrans::serializeTo(const AosBuffPtr &buff)
{
	bool rslt = AosTaskTrans::serializeTo(buff);
	aos_assert_r(rslt, false);
	return true;
}


AosTransPtr
AosSendClearIILEntryMapTrans::clone()
{
	return OmnNew AosSendClearIILEntryMapTrans(false);
}


bool
AosSendClearIILEntryMapTrans::proc()
{
	bool rslt = AosTaskMgr::getSelf()->clearIILEntryMap(mRdata);
	AosBuffPtr resp_buff = OmnNew AosBuff(10 AosMemoryCheckerArgs);
	resp_buff->setU8(rslt);
	sendResp(resp_buff);
	return true;
}

