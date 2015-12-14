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
// 02/19/2007: Created by Chen Ding
//
////////////////////////////////////////////////////////////////////////////
#ifndef Omn_RandomValueGen_TestCond_h
#define Omn_RandomValueGen_TestCond_h

#include "Parms/Ptrs.h"
#include "Util/RCObject.h"
#include "Util/String.h"

#if 0

class AosTestCond : public OmnRCObject
{

private:
	OmnString	mCode;
	OmnString	mErrmsg;

public:
	AosTestCond(const OmnString &name);
	virtual ~AosTestCond();

	static AosTestCondPtr createCond(const OmnXmlItemPtr &def);
	bool	config(const OmnXmlItemPtr &def);

	OmnString	genCode(const AosTesterGenPtr &gen);

private:
};

#endif

#endif
