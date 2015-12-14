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
#ifndef Omn_RandomValueGen_TestCheck_h
#define Omn_RandomValueGen_TestCheck_h

#include "Parms/Ptrs.h"
#include "Util/RCObject.h"
#include "Util/String.h"
#include "XmlParser/Ptrs.h"



class AosTestCheck : public OmnRCObject
{

private:

public:
	AosTestCheck(const OmnString &name);
	virtual ~AosTestCheck();

	static AosTestCheckPtr	createTestCheck(const OmnXmlItemPtr &);

private:
};

#endif

