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
// This class similates a constant. A constant can be integral, float, 
// string, or any other type. The constant is represented by AosValue. 
//   
//
// Modification History:
// Wednesday, January 02, 2008: Created by Allen Xu 
//
////////////////////////////////////////////////////////////////////////////
#include "Conditions/CondDataConstant.h"

#include "alarm/Alarm.h"
#include "Util/OmnNew.h"

AosCondDataConst::AosCondDataConst()
{
	mDataFlag = "Constant";
	mCondDataType = AosCondDataType::eConstant;
}


AosCondDataConst::~AosCondDataConst()
{
}


bool 
AosCondDataConst::getData(AosValuePtr& value, const AosExeDataPtr &)
{
	value = mValue;
	return true;
}


bool 
AosCondDataConst::serialize(TiXmlNode& node)
{
	// 
	// 	<CondDataConst>
	// 		<Value>
	// 			<ValueType>
	// 			<ByteString>
	// 		</Value>
	// 	</CondDataConst>
	//
	aos_assert_r(mValue, false);
	node.SetValue("CondDataConst");

	TiXmlElement value("Value");
	mValue->serialize(value);
	node.InsertEndChild(value);

	return true;
}


bool 
AosCondDataConst::deserialize(TiXmlNode* node)
{
	// 
	// For the format, refer to ::serialize(...)
	//
	aos_assert_r(node, false);
	aos_assert_r(node->ValueStr() == "CondDataConst", false);
	std::string type;
	aos_assert_r(type == "Constant", false);
	TiXmlNode *valueNode = node->FirstChild("Value");

	mValue = OmnNew AosValue();
	aos_assert_r(mValue, false);
	aos_assert_r(mValue->deserialize(valueNode), false);
	return true;
}

