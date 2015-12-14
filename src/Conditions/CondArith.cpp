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
// December 28, 2007: Created by Allen Xu 
//
////////////////////////////////////////////////////////////////////////////
#include "Conditions/CondArith.h"

#include "alarm/Alarm.h"
#include "Conditions/CondData.h"
#include "Util/OmnNew.h"

AosCondArith::AosCondArith()
{
}


AosCondArith::~AosCondArith()
{
}


bool 
AosCondArith::serialize(TiXmlNode& node)
{
	//
	// <"ArithCond">
	// 		<LeftCondData>
	// 		</LeftCondData>
	// 		<RightCondData>
	// 		</RightCondData>
	// </"ArithCond">
	//
	aos_assert_r(mLeftValue, false);
	aos_assert_r(mRightValue, false);
	node.SetValue("ArithCond");
	
	TiXmlElement child("");
	if (!mLeftValue->serialize(child))
	{
		OmnAlarm << "Failed to serialize left-hand operand for "
			"ArithCond!" << enderr;
		return false;
	}
	node.InsertEndChild(child);

	if (!mRightValue->serialize(child))
	{
		OmnAlarm << "Failed to serialize right-hand operand for "
			"ArithCond!" << enderr;
		return false;
	}
	node.InsertEndChild(child);

	return true;
}


bool 
AosCondArith::deserialize(TiXmlNode* node)
{
	//
	// <"ArithCond">
	// 		<LeftCondData>
	// 		</LeftCondData>
	// 		<RightCondData>
	// 		</RightCondData>
	// </"ArithCond">
	//
	aos_assert_r(node, false);
	TiXmlNode *child = node->FirstChild();
	if (!child)
	{
		OmnAlarm << "Parsing an ArithCond, expecting left-hand "
			<< "operand, but found none" << enderr;
		return false;
	}

	mLeftValue = AosCondData::factory(child);
	if (!mLeftValue)
	{
		OmnAlarm << "Parsing an ArithCond, expecting left-hand "
			<< "operand, Failed creating!" << enderr;
		return false;
	}

	child = child->NextSibling();
	if (!child)
	{
		OmnAlarm << "Parsing an ArithCond, expecting right-hand "
			<< "operand, but found none" << enderr;
		return false;
	}

	mRightValue = AosCondData::factory(child);
	if (!mRightValue)
	{
		OmnAlarm << "Parsing an ArithCond, expecting right-hand "
			<< "operand, but failed creating!" << enderr;
		return false;
	}

	return true;
}


void 
AosCondArith::setLeftValue(const AosCondDataPtr& condData)
{
	mLeftValue = condData;
}


void 
AosCondArith::setRightValue(const AosCondDataPtr& condData)
{
	mRightValue = condData;
}


void 
AosCondArith::getLeftValue(AosCondDataPtr& condData)
{
	condData = mLeftValue;
}


void 
AosCondArith::getRightValue(AosCondDataPtr& condData)
{
	condData = mRightValue;
}

