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
#include "Conditions/CondLess.h"

#include "alarm/Alarm.h"
#include "Conditions/CondData.h"
#include "Torturer/Command.h"


AosCondLess::AosCondLess()
{
}


AosCondLess::~AosCondLess()
{
}


bool 
AosCondLess::serialize(TiXmlNode& node)
{
//
// <CondLess>
// </CondLess>
//
	if (!AosCondArith::serialize(node))
	{
		return false;
	}
	node.SetValue("CondLess");
	return true;
}


bool 
AosCondLess::deserialize(TiXmlNode* node)
{
//
// <CondLess>
// </CondLess>
//
	if (node == NULL)
	{
		return false;
	}

	if (node->ValueStr() != "CondLess")
	{
		return false;
	}

	if (!AosCondArith::deserialize(node))
	{
		return false;
	}

	return true;
}


bool 
AosCondLess::check(const AosExeDataPtr &ed)
{
	AosValuePtr left, right;
	aos_assert_r(mLeftValue, false);
	aos_assert_r(mRightValue, false);
	aos_assert_r(mLeftValue->getData(left, ed), false);
	aos_assert_r(mRightValue->getData(right, ed), false);
	aos_assert_r(left, false);
	aos_assert_r(right, false);
	return (*left < *right);
}


bool 
AosCondLess::check(const AosValue& value)
{
	/*
	 * Temperarily commented out by Chen Ding, 01/25/2008
	 *
	if (!mLeftValue)
	{
		return false;
	}

	AosValuePtr left;
	if (!mLeftValue->getData(left))
	{
		return false;
	}
	return (*left < value);
	*/
	return false;
}


bool 
AosCondLess::check(const AosValue& valueLeft, const AosValue& valueRight)
{
	return (valueLeft < valueRight);
}
