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
#ifndef Omn_RandomValueGen_TestVar_h
#define Omn_RandomValueGen_TestVar_h

#include "Util/RCObject.h"
#include "Util/String.h"

#if 0
class AosTestVar : public OmnRCObject
{
public:
	enum VarType
	{
		eFuncReturnValue
	};

private:
	OmnString	mName;
	VarType		mType;
	OmnString	mDataType;

public:
	AosTestVar(const OmnString &name);
	virtual ~AosTestVar();

	static AosTestVarPtr	createTestVar(const OmnXmlItemPtr &def);

private:
};

#endif

#endif

