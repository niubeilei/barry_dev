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
#include "Conditions/CondGreater.h"

#include "alarm/Alarm.h"
#include "Conditions/CondData.h"

AosCondGreater::AosCondGreater()
{
}


AosCondGreater::~AosCondGreater()
{
}


bool 
AosCondGreater::serialize(TiXmlNode& node)
{
//
// <CondGreater>
// 		<LeftCondData>
// 		</LeftCondData>
// 		<RightCondData>
// 		</RightCondData>
// </CondGreater>
//
	if (!AosCondArith::serialize(node))
	{
		return false;
	}
	node.SetValue("CondGreater");
	return true;
}


bool 
AosCondGreater::deserialize(TiXmlNode* node)
{
//
// <CondGreater>
// 		<LeftCondData>
// 		</LeftCondData>
// 		<RightCondData>
// 		</RightCondData>
// </CondGreater>
//
	if (node == NULL)
	{
		return false;
	}

	if (node->ValueStr() != "CondGreater")
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
AosCondGreater::check(const AosExeDataPtr &ed)
{
	AosValuePtr left, right;
	aos_assert_r(ed, false);
	aos_assert_r(mLeftValue, false);
	aos_assert_r(mRightValue, false);
	aos_assert_r(mLeftValue->getData(left, ed), false);
	aos_assert_r(mRightValue->getData(right, ed), false);
	aos_assert_r(left, false);
	aos_assert_r(right, false);
	return (*left > *right);
}


bool 
AosCondGreater::check(const AosValue& value)
{
	/*
	 * Commented out by Chen Ding, 01/25/2008
	 *
	AosValuePtr left;
	aos_assert_r(ed, false);
	aos_assert_r(mLeftValue, false);
	aos_assert_r(mLeftValue->getData(left, ed), false);
	aos_assert_r(left, false);
	return (*left > value);
	*/
	return true;
}


bool 
AosCondGreater::check(const AosValue& valueLeft, const AosValue& valueRight)
{
	return (valueLeft > valueRight);
}
