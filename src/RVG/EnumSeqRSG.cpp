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
// This class takes a list of string values:
//   [(value1, weight1), (value2, weight2), ..., (valuen, weightn)]
// It randomly generates string values that consist of the values
// selected from the value list. Values are concatenated together. 
// Separators are inserted in between. The selected values may have 
// some constraints:
// 1. Number of Values: [min, max]
//    The generated string shall contain at least 'min' number of values and
//    at most 'max' number of values. 
// 2. Repeat: true|false
//    Values in a generated string may or may not be repeated.
// 3. Order: true|false
//    Values may be ordered or not ordered.
//
// Modification History:
// 11/27/07: Created by Allen Xu 
//
////////////////////////////////////////////////////////////////////////////
#include "RVG/EnumSeqRSG.h"


AosEnumSeqRSG::AosEnumSeqRSG()
{
	mRVGType = AosRVGType::eEnumSeqRSG;
	mSeperator = ' ';
	mMinNum = 1;
	mMaxNum = 10;
	mUnique = false;
	mOrdered = false;
}

AosEnumSeqRSG::~AosEnumSeqRSG()
{
}


bool 
AosEnumSeqRSG::nextValue(AosValue& value)
{
	u32 numValues = aos_next_integer<u32>(mMinNum, mMaxNum);
	if (numValues <= 0)
	{
		return false;
	}
	if (mUnique) 
	{
		if (mOrdered) 
		{
			return nextUniqueOrdered(value, numValues);
		}
		else
		{
			return nextUniqueNotOrdered(value, numValues);
		}
	}
	else
	{
		if (mOrdered)
		{
			return nextMultipleOrdered(value, numValues);	
		}
		else
		{
			return nextMultipleNotOrdered(value, numValues);
		}
	}
}


bool
AosEnumSeqRSG::nextUniqueOrdered(AosValue &value, const u32 numRepeats)
{
	OmnString destStr;
	AosValue destValue;
//	if (numRepeats > mEnumStrWeight.size())
//	{
//		OmnAlarm << "Not enough values to create unique sequence" << enderr;
//		return false;
//	}
	AosEnumStrWeightList::iterator iter;

	resetCreateTimes();

	//
	// make a EnumStrWeight list copy
	//
	AosEnumStrWeightList tmpList = mEnumStrWeight;

	//
	// make random strings, set createTimes
	//
	for (u32 i=0; i<numRepeats; i++)
	{
		getTotalWeight(tmpList);
		u32 weight = 1;
		u32 randValue = aos_next_integer<u32>(1, mTotalWeight);

		for (iter=tmpList.begin(); iter!=tmpList.end(); iter++)
		{
			if (iter->weight== 0)
			{
				continue;
			}
			if (weight <= randValue && randValue < (weight+(*iter).weight))
			{
				iter->weight = 0;
				iter->createTimes = 1;
				break;
			}
			weight += iter->weight;
		}
	}

	//
	// create ordered dest string
	//
	for (iter=tmpList.begin(); iter!=tmpList.end(); iter++)
	{
		if (iter->createTimes == 1)
		{
			//
			// generate a string, append a seperator
			//
			destStr += iter->enumStr;
			destStr += mSeperator;
		}
	}

	//
	// remove the end seperator
	//
	if (!destStr.empty())
	{
		destStr = destStr.substr(0, destStr.length()-1);
		value.setValue((void*)destStr.c_str(), destStr.length());
		return true;
	}

	return false;
}


bool 
AosEnumSeqRSG::nextUniqueNotOrdered(AosValue &value, const u32 numRepeats)
{
	//
	// get a sequence strings uniquely but not ordered
	//	
	OmnString destStr;
	AosValue destValue;
	AosEnumStrWeightList::iterator iter;

	resetCreateTimes();
	//
	// make a EnumStrWeight list copy
	//
	AosEnumStrWeightList tmpList = mEnumStrWeight;
	for (u32 i=0; i<numRepeats; i++)
	{
		getTotalWeight(tmpList);
		u32 weight = 1;
		u32 randValue = aos_next_integer<u32>(1, mTotalWeight);

		for (iter=tmpList.begin(); iter!=tmpList.end(); iter++)
		{
			if (iter->weight== 0)
			{
				continue;
			}
			if (weight <= randValue && randValue < (weight+(*iter).weight))
			{
				//
				// generate a not ordered string, append a seperator, and set its weight to 0
				//
				destStr += iter->enumStr;
				destStr += mSeperator;
				iter->weight = 0;
				iter->createTimes = 1;
				break;
			}
			weight += iter->weight;
		}
	}

	//
	// remove the end seperator
	//
	if (!destStr.empty())
	{
		destStr = destStr.substr(0, destStr.length()-1);
		value.setValue((void*)destStr.c_str(), destStr.length());
		return true;
	}
	
	return false;
}


bool
AosEnumSeqRSG::nextMultipleOrdered(AosValue& value, const u32 numRepeats)
{
	//
	// not unique but orderde
	//
	OmnString destStr;
	AosValue destValue;
	AosEnumStrWeightList::iterator iter;

	getTotalWeight(mEnumStrWeight);
	resetCreateTimes();
	//
	// create strings, set createTimes
	//
	for (u32 i=0; i<numRepeats; i++)
	{
		u32 weight = 1;
		u32 randValue = aos_next_integer<u32>(1, mTotalWeight);

		for (iter=mEnumStrWeight.begin(); iter!=mEnumStrWeight.end(); iter++)
		{
			if (iter->weight== 0)
			{
				continue;
			}
			if (weight <= randValue && randValue < (weight+(*iter).weight))
			{
				iter->createTimes ++;
				break;
			}
			weight += iter->weight;
		}
	}

	//
	// create ordered dest string
	//
	for (iter=mEnumStrWeight.begin(); iter!=mEnumStrWeight.end(); iter++)
	{
		for (u32 j=0; j<iter->createTimes; j++)
		{
			//
			// generate a string, append a seperator
			//
			destStr += iter->enumStr;
			destStr += mSeperator;
		}
	}
	//
	// remove the end seperator
	//
	if (!destStr.empty())
	{
		destStr = destStr.substr(0, destStr.length()-1);
		value.setValue((void*)destStr.c_str(), destStr.length());
		return true;
	}

	return false;
}


bool
AosEnumSeqRSG::nextMultipleNotOrdered(AosValue &value, const u32 numRepeats)
{
	OmnString destStr;
	AosValue destValue;
	AosEnumRSG::nextValue(destValue);
	destStr = destValue.toString();
	for (u32 i=1; i<numRepeats; i++)
	{
		AosEnumRSG::nextValue(destValue);
		destStr += mSeperator;
		destStr += destValue.toString();
	}

	value.setValue((void*)destStr.c_str(), destStr.length());
	return true;
}


bool 
AosEnumSeqRSG::createIncorrectRVGs(AosRVGPtrArray& rvgList)
{
	return true;
}


bool 
AosEnumSeqRSG::serialize(TiXmlNode& node)
{
//
// serialize into a xml node
//	<EnumSeqRSG>
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
//		<Seperator>a charactor</Seperator>
//		<MinNum>10</MinNum>
//		<MaxNum>100</MaxNum>
//		<Unique>false/true</Unique>
//		<Ordered>false/true</Ordered>
//		<ComposeWeight>100</ComposeWeight>
//	</EnumSeqRSG>
//
	if (!AosEnumRSG::serialize(node))
	{
		return false;
	}
	OmnString seperator;
	seperator.push_back(mSeperator);
	node.addElement("Seperator", seperator);
	node.addElement("MinNum", AosValue::itoa(mMinNum));
	node.addElement("MaxNum", AosValue::itoa(mMaxNum));
	node.addElement("Unique", mUnique ? "true" : "false");
	node.addElement("Ordered", mOrdered ? "true" : "false");
	return true;
}


bool 
AosEnumSeqRSG::deserialize(TiXmlNode* node)
{
//
// parse a xml node
//	<EnumSeqRSG>
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
//		<Seperator>a charactor</Seperator>
//		<MinNum>10</MinNum>
//		<MaxNum>100</MaxNum>
//		<Unique>false/true</Unique>
//		<Ordered>false/true</Ordered>
//	</EnumSeqRSG>
//
	if (! AosEnumRSG::deserialize(node))
	{
		return false;
	}
	
	//
	// parse Seperator, mandatory field
	//
	TiXmlHandle docHandle(node);
	TiXmlElement* element = docHandle.FirstChild("Seperator").ToElement();
	if (element)
	{
		const char* value = element->GetText();
		if (value)
		{
			mSeperator = value[0];
		}
	}
	else
	{
		return false;
	}
	//
	// parse MinNum, mandatory field
	//
	if (!node->getElementTextValue("MinNum", mMinNum))
	{
		OmnAlarm << "XML parse: MinNum must exist in EnumSeqRSG" << enderr;
		return false;
	}
	//
	// parse MaxNum, mandatory field
	//
	if (!node->getElementTextValue("MaxNum", mMaxNum))
	{
		OmnAlarm << "XML parse: MaxNum must exist in EnumSeqRSG" << enderr;
		return false;
	}
	//
	// parse Unique, mandatory field
	//
	if (!node->getElementTextValue("Unique", mUnique))
	{
		OmnAlarm << "XML parse: Unique flag must exist in CharsetRSG" << enderr;
		return false;
	}
	//
	// parse Ordered, mandatory field
	//
	if (!node->getElementTextValue("Ordered", mOrdered))
	{
		OmnAlarm << "XML Parse: Ordered flag must exist in CharsetRSG" << enderr;
		return false;
	}
	return true;
}


char 
AosEnumSeqRSG::getSeparator()
{
	return mSeperator;
}


u32 
AosEnumSeqRSG::getMinNum()
{
	return mMinNum;
}


u32 
AosEnumSeqRSG::getMaxNum()
{
	return mMaxNum;
}


bool 
AosEnumSeqRSG::getUniqueFlag()
{
	return mUnique;
}


bool 
AosEnumSeqRSG::getOrderedFlag()
{
	return mOrdered;
}


void 
AosEnumSeqRSG::setSeperator(const char sep)
{
	mSeperator = sep;
}


void 
AosEnumSeqRSG::setMinNum(const u32 min)
{
	mMinNum = min;
}


void 
AosEnumSeqRSG::setMaxNum(const u32 max)
{
	mMaxNum = max;
}


void 
AosEnumSeqRSG::setUniqueFlag(const bool flag)
{
	mUnique = flag;
}


void 
AosEnumSeqRSG::setOrderedFlag(const bool flag)
{
	mOrdered = flag;
}

