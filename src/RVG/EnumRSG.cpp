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
// This class randomly generates a string by randomly selecting a value
// from a value list:
//       [value1, weight1], 
//       [value2, weight2], 
//       ..., 
//       [valuen, weightn]
// The weights control how often a value is selected. 
//
// Modification History:
// 11/21/2007: Created by Allen Xu
//
////////////////////////////////////////////////////////////////////////////
#include "RVG/EnumRSG.h"


AosEnumRSG::AosEnumRSG():
mTotalWeight(0)
{
	mRVGType = AosRVGType::eEnumRSG;
}


AosEnumRSG::~AosEnumRSG()
{
}


// 
// we use an RIG to determine which value to select, 
// this function can be implemented very simple:
// 
// Assume:
//    [value1, weight1], 
//    [value2, weight2], 
//    ...
//    [valuen, weightn]
//
// Create an RIG:
//    [0, 0, weight1], 
//    [1, 1, weigh2], 
//    ..., 
//    [n-1, n-1, weighn]
//
// Note that not only this function is much simpler, but also 
// much faster (if we implement the RIG using the arrays.
//
bool 
AosEnumRSG::nextValue(AosValue& value)
{
	AosValue indexValue;
	mWeightHash.nextValue(indexValue);
	u32 index = indexValue.toUint32();
	if (index < 0 || index >= mEnumStrWeight.size())
	{
		return false;
	}
	
	value.setType(mValueType);
	value.setValue((void*)mEnumStrWeight[index].enumStr.c_str(), mEnumStrWeight[index].enumStr.length());
	
/*	u32 weight = 1;
	u32 randValue = aos_next_integer<u32>(1, mTotalWeight);

	AosEnumStrWeightList::iterator iter;
	for (iter=mEnumStrWeight.begin(); iter!=mEnumStrWeight.end(); iter++)
	{
		if (iter->weight== 0)
		{
			continue;
		}
		if (weight <= randValue && randValue < (weight+(*iter).weight))
		{
			value.setValue((void*)iter->enumStr.c_str(), iter->enumStr.length());
			value.setType(mValueType);
			break;
		}
		weight += iter->weight;
	}
*/
	return true;
}


bool 
AosEnumRSG::createIncorrectRVGs(AosRVGPtrArray& rvgList)
{
	return true;
}


// 
// we use an RIG to determine which value to select, 
// Need reconstruct when the string weight list changes.
// 
// Assume:
//    [value1, weight1], 
//    [value2, weight2], 
//    ...
//    [valuen, weightn]
//
// Create an RIG:
//    [0, 0, weight1], 
//    [1, 1, weigh2], 
//    ..., 
//    [n-1, n-1, weighn]
void 
AosEnumRSG::reconstructWeightHash()
{
	mWeightHash.clearIntegerPair();
	int size = mEnumStrWeight.size();
	for (int i=0; i<size; i++)
	{
		mWeightHash.setIntegerPair(i, i, mEnumStrWeight[i].weight);
	}
}


bool 
AosEnumRSG::serialize(TiXmlNode &node)
{
//
// serialize into a XML node 
// 	<EnumRSG>
// 		<Name>a_global_unique_name</Name>
// 		<ValueType>string</ValueType>
//		<ComposeWeight>100</ComposeWeight>
//		<ValueList>
//			<Value>
//				<EnumStr>string1</EnumStr>
//				<Weight>100</Weight>
//			</Value>
//			<Value>
//				<EnumStr>string2</EnumStr>
//				<Weight>100</Weight>
//			</Value>
//			...
//			<Value>
//				<EnumStr>stringn</EnumStr>
//				<Weight>100</Weight>
//			</Value>
//		</ValueList>
// 	</EnumRSG>
//
	if (! AosRVG::serialize(node))
	{
		return false;
	}
	//
	// add ValueList
	//
	TiXmlElement valueList("ValueList");
	AosEnumStrWeightList::iterator iter;
	for (iter=mEnumStrWeight.begin(); iter!=mEnumStrWeight.end(); iter++)
	{
		TiXmlElement value("Value");
		value.addElement("EnumStr", iter->enumStr);
		value.addElement("Weight", AosValue::itoa(iter->weight));
		valueList.InsertEndChild(value);
	}
	node.InsertEndChild(valueList);
	return true;
}


bool 
AosEnumRSG::deserialize(TiXmlNode* node)
{
//
// Parse from a XML node 
// 	<EnumRSG>
// 		<Name>a_global_unique_name</Name>
// 		<ValueType>string</ValueType>
//		<ValueList>
//			<Value>
//				<EnumStr>string1</EnumStr>
//				<Weight>100</Weight>
//			</Value>
//			<Value>
//				<EnumStr>string2</EnumStr>
//				<Weight>100</Weight>
//			</Value>
//			...
//			<Value>
//				<EnumStr>stringn</EnumStr>
//				<Weight>100</Weight>
//			</Value>
//		</ValueList>
//		<ComposeWeight>100</ComposeWeight>
// 	</EnumRSG>
//
	if (! AosRVG::deserialize(node))
	{
		return false;
	}
	//
	// parse value list
	//
	TiXmlHandle docHandle(node);
	TiXmlElement* element = docHandle.FirstChild("ValueList").ToElement();
	if (element)
	{
		OmnString enumStr;
		u32 weight;	
		TiXmlElement* child = element->FirstChildElement("Value");	
		for (; child!=NULL; child=child->NextSiblingElement("Value"))
		{
			if (child->getElementTextValue("EnumStr", enumStr))
			{
				if (child->getElementTextValue("Weight", weight))
				{
					setEnumString(enumStr, weight);
				}
			}
		}
	}
	return true;
}


// 
// Copy the enum string list to 'strLits'.
//
void 
AosEnumRSG::getEnumString(AosEnumStrList& strList)
{
	AosEnumStrWeightList::iterator iter;
	for (iter=mEnumStrWeight.begin(); iter!=mEnumStrWeight.end(); iter++)
	{
		strList.push_back(iter->enumStr);
	}
}


void 
AosEnumRSG::getEnumString(AosEnumStrWeightList& strWeightList)
{
	strWeightList = mEnumStrWeight;
}


// 
// Add a value (str, weight) to the enum list. If the name 
// already exists, update its weight. Otherwise, add the entry.
//
void 
AosEnumRSG::setEnumString(const OmnString &str, u32 weight)
{
	AosEnumStrWeightList::iterator iter;
	iter = std::find_if(mEnumStrWeight.begin(), mEnumStrWeight.end(), 
								bind2nd(FunctorFindEnumStr(), str));
	if (iter != mEnumStrWeight.end())
	{
		// 
		// Update the mTotalWeight, too.
		//
		mTotalWeight -= iter->weight;
		mTotalWeight += weight;
		iter->weight = weight;

		return;
	}

	// 
	// The entry is not in the list. Append it.
	//
	StrWeight strweight;
	strweight.enumStr = str;
	strweight.weight = weight;
	mEnumStrWeight.push_back(strweight);
	mTotalWeight += weight;
	reconstructWeightHash();
}


bool 
AosEnumRSG::removeEnumString(const OmnString &str)
{
	AosEnumStrWeightList::iterator iter = std::find_if(mEnumStrWeight.begin(),
														mEnumStrWeight.end(),
														bind2nd(FunctorFindEnumStr(), str));
	if (iter != mEnumStrWeight.end())
	{
		mEnumStrWeight.erase(iter);
		getTotalWeight(mEnumStrWeight);
		reconstructWeightHash();
		return true;
	}
	return false;
}


void 
AosEnumRSG::clearEnumString()
{
	mEnumStrWeight.clear();
}


u32 
AosEnumRSG::getTotalWeight(AosEnumStrWeightList& strWeightList)
{
	mTotalWeight = 0;
	AosEnumStrWeightList::iterator iter;
	for (iter=strWeightList.begin(); iter!=strWeightList.end(); iter++)
	{
		mTotalWeight += iter->weight;		
	}
	return mTotalWeight;
}

void 
AosEnumRSG::resetCreateTimes()
{
	AosEnumStrWeightList::iterator iter;
	for (iter=mEnumStrWeight.begin(); iter!=mEnumStrWeight.end(); iter++)
	{
		iter->createTimes = 0;		
	}
}


