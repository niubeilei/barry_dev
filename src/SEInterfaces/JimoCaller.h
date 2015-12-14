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
#ifndef Aos_SEInterfaces_JimoCaller_h
#define Aos_SEInterfaces_JimoCaller_h

#include "SEInterfaces/JimoCallData.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Rundata/Rundata.h"
#include "Rundata/Ptrs.h"

class AosCallData;
class AosJimoCall;

class AosJimoCaller : virtual public OmnRCObject
{
public:
	// virtual bool callFinished(AosRundata *rdata, const AosJimoCallData &call_data) = 0;
	// virtual bool callFailed(AosRundata *rdata, const AosJimoCallData &call_data) = 0;
	virtual bool callFinished(AosRundata *rdata, AosJimoCall &jimo_call) = 0;
};

#endif
