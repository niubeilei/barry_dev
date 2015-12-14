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
// 2014/11/25 Created by Chen Ding
// 2015/02/20 Moved to SEInterfaces by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_JimoCall_JimoCallerDummy_h
#define Aos_JimoCall_JimoCallerDummy_h

#include "SEInterfaces/JimoCallData.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Rundata/Rundata.h"
#include "Rundata/Ptrs.h"

class AosJimoCallerDummy : public AosJimoCaller 
{
	OmnDefineRCObject;

public:
	AosJimoCallerDummy() {}
	~AosJimoCallerDummy() {}

	virtual bool callFinished(
			AosRundata *rdata, 
			AosJimoCall &jimo_call)
	{ return true; }
};

#endif
