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
// 2014/11/30 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SEInterfaces_JimoCallPackage_h
#define Aos_SEInterfaces_JimoCallPackage_h

#include "Jimo/Jimo.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Rundata/Rundata.h"
#include "Rundata/Ptrs.h"

class AosJimoCall;

class AosJimoCallPackage : virtual public AosJimo
{
public:
	virtual bool jimoCall(AosRundata *rdata, AosJimoCall &jimo_call) = 0;
	virtual bool isValid() const = 0;
};

#endif
