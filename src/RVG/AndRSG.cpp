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

#include "RVG/AndRSG.h"

AosAndRSG::AosAndRSG()
{
	mRVGType = AosRVGType::eAndRSG;
	mOrdered = false;
}


AosAndRSG::~AosAndRSG()
{
}


bool 
AosAndRSG::nextValue(AosValue& value)
{
	OmnString destStr;
	AosValue tmpValue;
	for (AosRVGPtrList::iterator iter=mBRVGList.begin(); iter!=mBRVGList.end(); iter++)
	{
		if (!(*iter))
		{
			OmnAlarm << "The RSG pointer can not be empty" << enderr;
			return false;
		}
		(*iter)->nextValue(tmpValue);
		destStr += tmpValue.toString();
	}

	if (destStr.length() <= 0)
	{
		return false;
	}
	value.setValue((void*)destStr.data(), destStr.length());
	return true;
}


bool 
AosAndRSG::createIncorrectRVGs(AosRVGPtrArray& rvgList)
{
	return true;
}


bool 
AosAndRSG::serialize(TiXmlNode& node)
{
//
// serialize into a xml node
// <AndRSG>
// 		<Name>a_global_unique_name</Name>
// 		<ValueType>string</ValueType>
//		<ComposeWeight>100</ComposeWeight>
//		<RVGList>...</RVGList>
//		<Ordered>false/true</Ordered>
// </AndRSG>
//
	if (! AosComposeRSG::serialize(node))
	{
		return false;
	}
	//
	// add Ordered node
	//
	node.addElement("Ordered", mOrdered ? "true" : "false");
	return true;
}


bool 
AosAndRSG::deserialize(TiXmlNode* node)
{
//
// parse a xml node
// <AndRSG>
// 		<Name>a_global_unique_name</Name>
// 		<ValueType>string</ValueType>
//		<ComposeWeight>100</ComposeWeight>
//		<RVGList>...</RVGList>
//		<Ordered>false/true</Ordered>
// </AndRSG>
//
	if (! AosComposeRSG::deserialize(node))
	{
		return false;
	}
	//
	// parse Ordered
	//
	if (!node->getElementTextValue("Ordered", mOrdered))
	{
		OmnAlarm << "XML Parse: Ordered flag must exist in AndRSG" << enderr;
		return false;
	}
	return true;
}


void 
AosAndRSG::setOrderedFlag(bool flag)
{
	mOrdered = flag;
}


bool 
AosAndRSG::getOrderedFlag()
{
	return mOrdered;
}



