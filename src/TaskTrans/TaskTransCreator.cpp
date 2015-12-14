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
#include "TaskTrans/TaskTransCreator.h"


bool 
AosTaskTransCreator::registerTaskTrans(const AosTaskTransObjPtr &cacher)
{
}


AosTaskTransObjPtr 
AosTaskTransCreator::createTaskTrans(
		const AosJobObjPtr &job,
		const AosXmlTagPtr &tag, 
		const AosRundataPtr &rdata)
{
	OmnNotImplementedYet;
	return 0;
}


AosTaskTransObjPtr 
AosTaskTransCreator::serializeFrom(
		const AosBuffPtr &buff, 
		const AosRundataPtr &rdata)
{
	OmnNotImplementedYet;
	return 0;
}


bool
AosTaskTransCreator::setCreator()
{
	AosTaskTransCreatorObjPtr creator = OmnNew AosTaskTransCreator();
	AosTaskTransObj::setCreator(creator);
	return true;
}

