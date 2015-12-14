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
// 01/28/2008: Created by Chen Ding
//
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_Conditions_CondAnd_H
#define Aos_Conditions_CondAnd_H

#include "Conditions/Condition.h"
#include <vector>

class AosCondAnd : public AosCondition
{
	OmnDefineRCObject;
private:
	AosConditionVector	mConditions;

public:
	AosCondAnd();
	~AosCondAnd();
	virtual bool serialize(TiXmlNode& node);
	virtual bool deserialize(TiXmlNode* node);

	virtual bool check(const AosExeDataPtr &ed);
};
#endif

