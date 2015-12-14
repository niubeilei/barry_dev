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
// 2015/04/01 Created by Xia Fan
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_SEInterfaces_JimoProgMgrObj_h
#define AOS_SEInterfaces_JimoProgMgrObj_h

#include "Jimo/Jimo.h"
#include "SEInterfaces/Ptrs.h"
#include "Util/String.h"

class AosRundata;

class AosJimoProgMgrObj :virtual  public AosJimo
{
public:
	AosJimoProgMgrObj(const int version);

	virtual AosJimoProgObjPtr getJimoProg(AosRundata *rdata, const OmnString &name) = 0;
	virtual AosJimoProgObjPtr retrieveJimoProg(AosRundata *rdata, const OmnString &name) const = 0;
};
#endif

