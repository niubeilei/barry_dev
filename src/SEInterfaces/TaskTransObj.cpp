////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
//
// Modification History:
// 08/05/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "SEInterfaces/TaskTransObj.h"

#include "Rundata/Rundata.h"
#include "SEInterfaces/JobObj.h"
#include "SEInterfaces/TaskDataObj.h"
#include "SEInterfaces/TaskTransCreatorObj.h"


AosTaskTransCreatorObjPtr AosTaskTransObj::smCreator;


bool
AosTaskTransObj::registerTaskTrans(const AosTaskTransObjPtr &task)
{
	aos_assert_r(smCreator, false);
	return smCreator->registerTaskTrans(task);
}


AosTaskTransObjPtr 
AosTaskTransObj::createTaskTrans(
		const AosJobObjPtr &job,
		const AosXmlTagPtr &tag, 
		const AosRundataPtr &rdata)
{
	aos_assert_rr(smCreator, rdata, 0);
	return smCreator->createTaskTrans(job, tag, rdata);
}


AosTaskTransObjPtr 
AosTaskTransObj::serializeFromStatic(
		const AosBuffPtr &buff,
		const AosRundataPtr &rdata)
{
	aos_assert_rr(smCreator, rdata, 0);
	return smCreator->serializeFrom(buff, rdata);
}

