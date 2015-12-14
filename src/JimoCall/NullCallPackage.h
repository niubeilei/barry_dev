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
#ifndef Aos_JimoCall_JimoNullCallPackage_h
#define Aos_JimoCall_JimoNullCallPackage_h

#include "SEInterfaces/JimoCallPackage.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Rundata/Rundata.h"
#include "Rundata/Ptrs.h"

class AosNullJimoCallPackage : public AosJimoCallPackage
{
	OmnDefineRCObject;

public:
	AosNullJimoCallPackage(const int version);

	virtual bool jimoCall(AosRundata *rdata, AosJimoCall &jimo_call);
	virtual bool isValid() const {return false;}
	virtual AosJimoPtr cloneJimo() const;
};

#endif
