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
// Monday, December 17, 2007: Created by Allen Xu 
//
////////////////////////////////////////////////////////////////////////////
#include "Conditions/Condition.h"

#include "alarm/Alarm.h"
#include "Conditions/CondUtil.h"
#include "Conditions/CondInField.h"
#include "Conditions/CondNotInField.h"
#include "Conditions/CondEqual.h"
#include "Conditions/CondNotEqual.h"
#include "Conditions/CondGreater.h"
#include "Conditions/CondGreaterEqual.h"
#include "Conditions/CondLess.h"
#include "Conditions/CondLessEqual.h"
#include "Util/OmnNew.h"

AosCondition::AosCondition()
{
}


AosCondition::~AosCondition()
{
}


AosConditionPtr 
AosCondition::factory(TiXmlNode* node)
{
	if (!node)
	{
		return NULL;
	}
	AosConditionPtr ptr = NULL;
	AosConditionType::E index = AosConditionType::str2Enum(node->ValueStr());
	switch (index)
	{
	case AosConditionType::eInField:
		 ptr = OmnNew AosCondInField();
		 break;

	case AosConditionType::eNotInField:
		 ptr = OmnNew AosCondNotInField();
		 break;

	case AosConditionType::eEqual:
		 ptr = OmnNew AosCondEqual();
		 break;

	case AosConditionType::eNotEqual:
		 ptr = OmnNew AosCondNotEqual();
		 break;

	case AosConditionType::eGreater:
		 ptr = OmnNew AosCondGreater();
		 break;

	case AosConditionType::eGreaterEqual:
		 ptr = OmnNew AosCondGreaterEqual();
		 break;

	case AosConditionType::eLess:
		 ptr = OmnNew AosCondLess();
		 break;

	case AosConditionType::eLessEqual:
		 ptr = OmnNew AosCondLessEqual();
		 break;

	default:
		 OmnAlarm << "No such condition type" << enderr;
		 break;
	}

	if (ptr)
	{
		ptr->deserialize(node);
	}

	return ptr;
}

