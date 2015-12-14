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
// Modification History:
// 2014/11/08 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_CubeComm_CubeListener_h
#define Aos_CubeComm_CubeListener_h

#include "Util/RCObject.h"
#include "Util/String.h"
#include "UtilComm/Ptrs.h"



class AosCubeListener : public virtual OmnRCObject
{
public:
	virtual bool		msgRead(const OmnConnBuffPtr &buff) = 0;
	virtual OmnString	getListenerName() const = 0;
	virtual void 		readingFailed()  = 0;
};
#endif
