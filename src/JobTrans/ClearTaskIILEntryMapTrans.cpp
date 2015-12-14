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
#include "JobTrans/ClearTaskIILEntryMapTrans.h"

#include "API/AosApi.h"
#include "XmlUtil/XmlTag.h"
#include "SEInterfaces/TaskObj.h"
#include "SEInterfaces/TaskMgrObj.h"
#include "Thread/ThreadPool.h"
#include "TaskMgr/TaskThrd.h"
#include "Debug/Debug.h"
#include "IILEntryMap/IILEntryMapMgr.h"

AosClearTaskIILEntryMapTrans::AosClearTaskIILEntryMapTrans(const bool regflag)
:
AosTaskTrans(AosTransType::eClearTaskIILEntryMap, regflag)
{
}


AosClearTaskIILEntryMapTrans::AosClearTaskIILEntryMapTrans(
		const int svr_id,
		const u32 to_proc_id)
:
AosTaskTrans(AosTransType::eClearTaskIILEntryMap, svr_id, to_proc_id, false, true)
{
}

AosClearTaskIILEntryMapTrans::~AosClearTaskIILEntryMapTrans()
{
}

bool
AosClearTaskIILEntryMapTrans::serializeFrom(const AosBuffPtr &buff)
{
	bool rslt = AosTaskTrans::serializeFrom(buff);
	aos_assert_r(rslt, false);
	return true;
}


bool
AosClearTaskIILEntryMapTrans::serializeTo(const AosBuffPtr &buff)
{
	bool rslt = AosTaskTrans::serializeTo(buff);
	aos_assert_r(rslt, false);
	return true;
}


AosTransPtr
AosClearTaskIILEntryMapTrans::clone()
{
	return OmnNew AosClearTaskIILEntryMapTrans(false);
}


bool
AosClearTaskIILEntryMapTrans::proc()
{
	AosIILEntryMapMgr::clear();
	AosBuffPtr resp_buff = OmnNew AosBuff(10 AosMemoryCheckerArgs);
	resp_buff->setU8(true);
	sendResp(resp_buff);
	return true;
}
