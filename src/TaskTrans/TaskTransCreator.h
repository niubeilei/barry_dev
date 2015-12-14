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
#ifndef Aos_TaskTrans_TaskTransCreator_h
#define Aos_TaskTrans_TaskTransCreator_h

#include "SEInterfaces/TaskTransCreatorObj.h"


class AosTaskTransCreator : virtual public AosTaskTransCreatorObj
{
public:
	virtual bool registerTaskTrans(const AosTaskTransObjPtr &cacher);

	virtual AosTaskTransObjPtr createTaskTrans(
					const AosJobObjPtr &job,
					const AosXmlTagPtr &tag, 
					const AosRundataPtr &rdata);

	virtual AosTaskTransObjPtr serializeFrom(
					const AosBuffPtr &buff, 
					const AosRundataPtr &rdata);
};
#endif

