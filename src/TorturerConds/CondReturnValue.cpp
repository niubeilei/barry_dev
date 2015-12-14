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
// This class implements the conditions that check function return
// values. The following conditions are currently supported:
// 	1. eGivenValueIfCorrect
// 	   If all the parameters were constructed correctly, the function
// 	   should always return a given value, which is stored in 
// 	   'mExpected'.
//   
//
// Modification History:
// 06/08/2007: Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "TorturerConds/CondReturnValue.h"

#include "alarm/Alarm.h"
#include "Debug/Debug.h"
#include "TorturerConds/CondData.h"
#include "XmlParser/XmlItem.h"


AosCondReturnValue::AosCondReturnValue(
			const CondType type, 
			const OmnString &expected)
:
mType(type),
mExpected(expected)
{
}


AosCondReturnValue::~AosCondReturnValue()
{
}


bool
AosCondReturnValue::check(const AosCondData &data, 
						  bool &rslt, 
						  OmnString &errmsg) const
{
	switch (mType)
	{
	case eGivenValueIfCorrect:
	     rslt = (mExpected == data.getReturnValue());
		 if (!rslt)
		 {
		 	(errmsg = "Expecting function return: ") << mExpected
				<< ", but actual: " << data.getReturnValue();
		 }
		 return true;

	default:
		 OmnAlarm << "Unrecognized condition type: " << mType << enderr;
		 return false;
	}

	OmnAlarm << "Should never come to this point" << enderr;
	return false;
}


bool
AosCondReturnValue::parseCond(const OmnXmlItemPtr &def, 
						  OmnDynArray<AosTortCondPtr> &conds)
{
	OmnString tagId = def->getTag();
	if (tagId == "ReturnCheck1")
	{
		OmnString expected = def->getStr();
		AosCondReturnValuePtr cond = 
			OmnNew AosCondReturnValue(eGivenValueIfCorrect, expected);
		aos_assert_r(cond, false);
		conds.append(cond);
		return true;
	}

	OmnAlarm << "Unrecognized Function Return Value Condition: " 
		<< tagId << enderr;
	return false;
}

