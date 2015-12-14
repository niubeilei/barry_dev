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
// Monday, December 03, 2007: Created by Allen Xu 
//
////////////////////////////////////////////////////////////////////////////

#include "RVG/OrRSG.h"

AosOrRSG::AosOrRSG()
{
	mRVGType = AosRVGType::eOrRSG;
}


AosOrRSG::~AosOrRSG()
{
}


bool 
AosOrRSG::nextValue(AosValue& value)
{
	u32 totalWeight = getTotalComposeWeight(mBRVGList);

	u32 weight = 1;
	u32 randValue = aos_next_integer<u32>(1, totalWeight);

	for (AosRVGPtrList::iterator iter=mBRVGList.begin(); iter!=mBRVGList.end(); iter++)
	{
		if (!(*iter))
		{
			OmnAlarm << "The RSG pointer can not be empty" << enderr;
			return false;
		}
		u32 composeWeight = (*iter)->getComposeWeight();
		if (composeWeight == 0)
		{
			continue;
		}
		if (weight <= randValue && randValue < (weight+composeWeight))
		{
			(*iter)->nextValue(value);
			break;
		}
		weight += composeWeight;
	}
	return true;
}


bool 
AosOrRSG::createIncorrectRVGs(AosRVGPtrArray& rvgList)
{
	return true;
}


bool 
AosOrRSG::serialize(TiXmlNode& node)
{
//
// serialize into a xml node
// <OrRSG>
// 		<Name>a_global_unique_name</Name>
// 		<ValueType>string</ValueType>
//		<ComposeWeight>100</ComposeWeight>
//		<RVGList>...</RVGList>
// </OrRSG>
//
	if (! AosComposeRSG::serialize(node))
	{
		return false;
	}
	return true;
}


bool 
AosOrRSG::deserialize(TiXmlNode* node)
{
//
// parse a xml node
// <OrRSG>
// 		<Name>a_global_unique_name</Name>
// 		<ValueType>string</ValueType>
//		<ComposeWeight>100</ComposeWeight>
//		<RVGList>...</RVGList>
// </OrRSG>
//
	if (! AosComposeRSG::deserialize(node))
	{
		return false;
	}
	return true;
}

