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
// Condition is used to control the generated value
// whether it satisfies this condition. When we generate
// an incorrect value by RVG for a parameter, we will check all its  
// incorrect conditions. As the same, when generate an correct value
// we need check all its correct conditions.
// 
// Now the conditions listed as following:
//		Not in field,
//		In field,
//		Not in record,
//		In record,
//		Not equal,
//		Greater,
//		Less,
//		Euqal,
//		GreaterEqual,
//		LessEqual,
//		...
//
// Modification History:
// 12/02/2007: Created by Chen Ding
// 12/17/2007: Modified by Allen Xu
// 
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_Conditions_Condition_h
#define Aos_Conditions_Condition_h

#include "Util/RCObject.h"
#include "RVG/CommonTypes.h"
#include "Conditions/Ptrs.h"

typedef std::vector<AosConditionPtr>	AosConditionVector;

//
// This is  base class of all conditions 
//
class AosCondition : virtual public OmnRCObject
{
public:
	AosCondition();
	virtual ~AosCondition();
	virtual bool serialize(TiXmlNode& node) = 0;
	virtual bool deserialize(TiXmlNode* node) = 0;

	virtual bool check(const AosExeDataPtr &ed) = 0;

	static AosConditionPtr factory(TiXmlNode* node);
};

#endif

