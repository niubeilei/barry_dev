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
#include "JobTrans/PauseTaskTrans.h"

#include "XmlUtil/XmlTag.h"
#include "Job/JobMgr.h"


AosPauseTaskTrans::AosPauseTaskTrans(const bool regflag)
:
AosTaskTrans(AosTransType::ePauseTask, regflag)
{
}


AosPauseTaskTrans::AosPauseTaskTrans(const int server_id)
:
AosTaskTrans(AosTransType::ePauseTask, server_id, false, false)
{
}


AosPauseTaskTrans::~AosPauseTaskTrans()
{
}


bool
AosPauseTaskTrans::serializeFrom(const AosBuffPtr &buff)
{
	bool rslt = AosTaskTrans::serializeFrom(buff);
	aos_assert_r(rslt, false);
	return true;
}


bool
AosPauseTaskTrans::serializeTo(const AosBuffPtr &buff)
{
	bool rslt = AosTaskTrans::serializeTo(buff);
	aos_assert_r(rslt, false);
	return true;
}


AosTransPtr
AosPauseTaskTrans::clone()
{
	return OmnNew AosPauseTaskTrans(false);
}


bool
AosPauseTaskTrans::proc()
{
	OmnNotImplementedYet;
	return false;
}


