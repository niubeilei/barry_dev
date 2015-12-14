////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// Modification History:
// 2013/06/02 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#if 0
#include "SEInterfaces/JobSplitterObj.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Rundata/Rundata.h"


AosJobSplitterObjPtr 	AosJobSplitterObj::smObject;


bool 
AosJobSplitterObj::initStatic()
{
	aos_assert_r(smObject, false);
	return smObject->init();
}


AosJobSplitterObjPtr 
AosJobSplitterObj::createJobSplitterStatic(
	   	const AosXmlTagPtr &sdoc,
		map<OmnString, OmnString> &job_env,
		const AosRundataPtr &rdata)
{
	aos_assert_r(smObject, 0);
	return smObject->createJobSplitter(sdoc, job_env, rdata);
}

#endif
