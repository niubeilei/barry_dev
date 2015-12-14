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
// 2014/12/13 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_JimoCall_JimoCallWaiter_h
#define Aos_JimoCall_JimoCallWaiter_h

#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Rundata/Rundata.h"
#include "Rundata/Ptrs.h"

class AosJimoCall;

class AosJimoCallWaiter 
{
public:
	static void wait(AosJimoCall &jimo_call, const int timer);

	static void	setWait(AosJimoCall &jimo_call);
};

#endif
