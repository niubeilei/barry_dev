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
#include "JobTrans/ResumeTaskTrans.h"

#include "XmlUtil/XmlTag.h"
#include "Job/JobMgr.h"


AosResumeTaskTrans::AosResumeTaskTrans(const bool regflag)
:
AosTaskTrans(AosTransType::eResumeTask, regflag)
{
}


AosResumeTaskTrans::AosResumeTaskTrans(
		const int svr_id)
:
AosTaskTrans(AosTransType::eResumeTask, svr_id, false, false)
{
}


AosResumeTaskTrans::~AosResumeTaskTrans()
{
}


bool
AosResumeTaskTrans::serializeFrom(const AosBuffPtr &buff)
{
	bool rslt = AosTaskTrans::serializeFrom(buff);
	aos_assert_r(rslt, false);
	return true;
}


bool
AosResumeTaskTrans::serializeTo(const AosBuffPtr &buff)
{
	bool rslt = AosTaskTrans::serializeTo(buff);
	aos_assert_r(rslt, false);
	return true;
}


AosTransPtr
AosResumeTaskTrans::clone()
{
	return OmnNew AosResumeTaskTrans(false);
}


bool
AosResumeTaskTrans::proc()
{
	OmnNotImplementedYet;
	return false;
}


