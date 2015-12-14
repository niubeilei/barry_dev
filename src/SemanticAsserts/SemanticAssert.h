////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: Ptrs.h
// Description:
//   
//
// Modification History:
// 12/02/2007: Created by Chen Ding
// 
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SemanticAsserts_SemanticAssert_h
#define Aos_SemanticAsserts_SemanticAssert_h

#include "Conditions/CondUtil.h"
#include "Event/EventUtil.h"
#include "Util/RCObject.h"

class AosSemanticAssert : virtual public OmnRCObject
{
protected:
	AosConditionList	mConditions;
	AosEventList		mEvents;

public:
	AosSemanticAssert(AosConditionList &conditions, 
					  AosEventList &events);
	~AosSemanticAssert();
};

#endif

