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
// This class randomly generates character strings based on a charset.
// A charset is defined as:
// 		[(char, weight), 
// 		 (char, weight),
// 		 (char, weight),
// 		 ...
// 		 (char, weight)]
// When generating a string, it first randomly determines the length
// of the string. It then randomly select characters from the charset
// based on the weights. 
//
// Additional Attributes:
// mUnique: Indicates whether characters can be repeated. 
// mOrder:  Indicates whether characters in a generated string should
//          be kept the same order as they appear in the character set. 
//
// Modification History:
// 11/21/2007: Created by Allen Xu
//
////////////////////////////////////////////////////////////////////////////
#include "RVG/CharsetRSG.h"

AosCharsetRSG::AosCharsetRSG()
{
	mRVGType = AosRVGType::eCharsetRSG;
	mMinLen = 1;
	mMaxLen = 10;
	mUnique = false;
	mOrdered = false;
}


AosCharsetRSG::~AosCharsetRSG()
{
}


bool 
AosCharsetRSG::nextValue(AosValue& value)
{
	// 
	// Chen Ding
	// Note that string length should be implemented by an RIG. 
	// Simply randomnize the length is not good enough. For instance,
	// if "user_name" maximum length is 128 and minimum length is 1, 
	// if we randomize it in the range [1, 128], most user_names are
	// very long (average is 64). Instead, users may want to define an
	// RIG:
	//       [1, 10, 100], 
	//       [11, 20, 50], 
	//       [21, 50, 10], 
	//       [51, 128, 1]
	//
	u32 len = aos_next_integer<u32>(mMinLen, mMaxLen);
	if (len <= 0)
	{
		return false;
	}
	OmnString destStr;
	int bRet;
	if (mUnique) 
	{
		if (mOrdered) 
		{
			bRet = nextUniqueOrdered(destStr, len);
		}
		else
		{
			bRet = nextUniqueNotOrdered(destStr, len);
		}
	}
	else
	{
		if (mOrdered)
		{
			bRet = nextMultipleOrdered(destStr, len);	
		}
		else
		{
			bRet = nextMultipleNotOrdered(destStr, len);
		}
	}

	if (bRet)
	{
		value.setValue((void*)destStr.data(),destStr.length());
	}

	return bRet;
}


bool 
AosCharsetRSG::createIncorrectRVGs(AosRVGPtrArray& rvgList)
{
	return true;
}


bool 
AosCharsetRSG::serialize(TiXmlNode& node)
{
//
// serialize into a xml node
//	<CharsetRSG>
// 		<Name>a_global_unique_name</Name>
// 		<ValueType>string</ValueType>
//		<ValueList>
//			<Value>
//				<Char>a</Char>
//				<Weight>100</Weight>
//			</Value>
//			<Value>
//				<Char>b</Char>
//				<Weight>100</Weight>
//			</Value>
//			...
//			<Value>
//				<Char>$</Char>
//				<Weight>100</Weight>
//			</Value>
//		</ValueList>
//		<MinLen>10</MinLen>
//		<MaxLen>100</MaxLen>
//		<Unique>false/true</Unique>
//		<Ordered>false/true</Ordered>
//		<ComposeWeight>100</ComposeWeight>
//	</CharsetRSG>
//
	OmnNotImplementedYet;
	/*
	 * Trying to change std::string to OmnString, but the line below
	 * calls 'push_back(...)'. Need to work it out. Chen Ding, 12/16/2008
	if (! AosRVG::serialize(node))
	{
		return false;
	}
	//
	// add ValueList
	//
	TiXmlElement valueList("ValueList");
	AosCharsetWeightList::iterator iter;
	for (iter=mCharWeightList.begin(); iter!=mCharWeightList.end(); iter++)
	{
		TiXmlElement value("Value");
		OmnString str;
		str.push_back(iter->mChar);
		value.addElement("Char", str);
		value.addElement("Weight", AosValue::itoa(iter->mWeight));
		valueList.InsertEndChild(value);
	}
	node.InsertEndChild(valueList);

	node.addElement("MinLen", AosValue::itoa(mMinLen));
	node.addElement("MaxLen", AosValue::itoa(mMaxLen));
	node.addElement("Unique", mUnique ? "true" : "false");
	node.addElement("Ordered", mOrdered ? "true" : "false");
	*/
	return true;
}


bool
AosCharsetRSG::deserialize(TiXmlNode* node)
{
//
// parse a xml node
//	<CharsetRSG>
// 		<Name>a_global_unique_name</Name>
// 		<ValueType>string</ValueType>
//		<ComposeWeight>100</ComposeWeight>
//		<ValueList>
//			<Value>
//				<Char>a</Char>
//				<Weight>100</Weight>
//			</Value>
//			<Value>
//				<Char>b</Char>
//				<Weight>100</Weight>
//			</Value>
//			...
//			<Value>
//				<Char>$</Char>
//				<Weight>100</Weight>
//			</Value>
//		</ValueList>
//		<MinLen>10</MinLen>
//		<MaxLen>100</MaxLen>
//		<Unique>false/true</Unique>
//		<Ordered>false/true</Ordered>
//	</CharsetRSG>
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
		OmnString str;
		u32 weight;	
		TiXmlElement* child = element->FirstChildElement("Value");	
		for (; child!=NULL; child=child->NextSiblingElement("Value"))
		{
			TiXmlElement* tmp = child->FirstChildElement("Char");
			if (tmp)
			{
				str = (tmp->GetText()) ? tmp->GetText() : "";	
				tmp = child->FirstChildElement("Weight");
				if (tmp)
				{
					weight = atoi(tmp->GetText() ? tmp->GetText() : "0");
					if (str.length()>0)
					{
						addChar(str.data()[0], weight);
					}
				}
			}
		}
	}
	
	//
	// parse MinLen, mandatory field
	//
	if (!node->getElementTextValue("MinLen", mMinLen))
	{
		OmnAlarm << "XML parse: MinLen must exist in CharsetRSG" << enderr;
		return false;
	}
	//
	// parse MaxLen
	//
	if (!node->getElementTextValue("MaxLen", mMaxLen))
	{
		OmnAlarm << "XML parse: MaxLen must exist in CharsetRSG" << enderr;
		return false;
	}
	//
	// parse Unique
	//
	if (!node->getElementTextValue("Unique", mUnique))
	{
		OmnAlarm << "XML parse: Unique flag must exist in CharsetRSG" << enderr;
		return false;
	}
	//
	// parse Ordered
	//
	if (!node->getElementTextValue("Ordered", mOrdered))
	{
		OmnAlarm << "XML Parse: Ordered flag must exist in CharsetRSG" << enderr;
		return false;
	}

	return true;
}


//
// get a charset string or all charset and weight
//
void 
AosCharsetRSG::getCharset(OmnString &str)
{
	OmnNotImplementedYet;
	/*
	 * Trying to replace std::string to OmnString, but this code was broken.
	 * Chen Ding, 12/16/2008
	AosCharsetWeightList::iterator iter;
	for (iter=mCharWeightList.begin(); iter!=mCharWeightList.end(); iter++)
	{
		str += iter->mChar;
	}
	*/
}


void 
AosCharsetRSG::getCharset(AosCharsetWeightList& charweight)
{
	charweight = mCharWeightList;
}

	
//
// add new char and its weight, if the charactor exists then modify its weight
//
void 
AosCharsetRSG::addChar(const char c, const int weight)
{	
	AosCharsetWeightList::iterator iter;
	iter = std::find_if(mCharWeightList.begin(),
								   mCharWeightList.end(),
								   bind2nd(FunctorFindChar(), c));
	if (iter != mCharWeightList.end())
	{
		iter->mWeight = weight;
	}
	else
	{
		CharWeight charWeight;
		charWeight.mChar = c;
		charWeight.mWeight = weight;
		mCharWeightList.push_back(charWeight);
	}
	getTotalWeight();
}


void 
AosCharsetRSG::addCharset(const OmnString & str, const int weight)
{
	const char *data = str.data();
	int i;
	for (i=0; i<str.length(); i++)
	{
		addChar(data[i], weight);	
	}
}


// void 
// AosCharsetRSG::addCharset(const AosCharsetWeightList& charweight)
// {
// }


//
// delete one or more charactors
//
bool 
AosCharsetRSG::delChar(const char c)
{
	AosCharsetWeightList::iterator iter;
	iter = std::find_if(mCharWeightList.begin(),
								   mCharWeightList.end(),
								   bind2nd(FunctorFindChar(), c));
	if (iter != mCharWeightList.end())
	{
		mCharWeightList.erase(iter);
		return true;
	}
	return false;
}


bool
AosCharsetRSG::delCharset(const OmnString &str)
{
	OmnNotImplementedYet;
	/*
	 * Need to conver the std::string to OmnString, Chen Ding, 12/16/2008
	for (OmnString std::string::const_iterator iter=str.begin(); iter!=str.end(); iter++)
	{
		if (!delChar((const char)(*iter)))
		{
			return false;
		}
	}
	*/
	return true;
}

u32 
AosCharsetRSG::getMinLen()
{
	return mMinLen;
}


u32 
AosCharsetRSG::getMaxLen()
{
	return mMaxLen;
}


bool 
AosCharsetRSG::getUniqueFlag()
{
	return mUnique;
}


bool 
AosCharsetRSG::getOrderedFlag()
{
	return mOrdered;
}


void 
AosCharsetRSG::setMinLen(const u32 min)
{
	mMinLen = min;
}


void 
AosCharsetRSG::setMaxLen(const u32 max)
{
	mMaxLen = max;
}


void 
AosCharsetRSG::setUniqueFlag(const bool flag)
{
	mUnique = flag;
}


void 
AosCharsetRSG::setOrderedFlag(const bool flag)
{
	mOrdered = flag;
}


// 
// Chen Ding
// Please add a description about how you generate unique and ordered
// strings. 
//
bool 
AosCharsetRSG::nextUniqueOrdered(OmnString &destStr, const u32 len)
{
	if (len > mCharWeightList.size())
	{
		OmnAlarm << "Not enough values to create unique string" << enderr;
		return false;
	}
	AosCharsetWeightList::iterator iter;

	resetCreateTimes();

	//
	// make a EnumStrWeight list copy for restoring later.
	//
	AosCharsetWeightList tmpList = mCharWeightList;

	//
	// randomly get chars, set createTimes
	//
	for (u32 i=0; i<len; i++)
	{
		getTotalWeight();
		u32 weight = 1;
		u32 randValue = aos_next_integer<u32>(1, mTotalWeight);

		for (iter=mCharWeightList.begin(); iter!=mCharWeightList.end(); iter++)
		{
			if (iter->mWeight== 0)
			{
				continue;
			}
			if (weight <= randValue && randValue <= (weight+(*iter).mWeight))
			{
				iter->mWeight = 0;
				iter->mCreateTimes = 1;
				break;
			}
			weight += iter->mWeight;
		}
	}

	OmnNotImplementedYet;
	/*
	 * Trying to replace std::string with OmnString, but broke the following. 
	 * Chen Ding, 12/16/2008
	//
	// create ordered dest string
	//
	for (iter=mCharWeightList.begin(); iter!=mCharWeightList.end(); iter++)
	{
		if (iter->mCreateTimes == 1)
		{
			destStr += iter->mChar;
		}
	}
	*/

	//
	// restore the CharWeight list
	//
	mCharWeightList.swap(tmpList);
	return true;
}


// 
// Chen Ding
// Please add a description about how you generate unique and
// not ordered strings.
//
bool 
AosCharsetRSG::nextUniqueNotOrdered(OmnString &destStr, const u32 len)
{
	AosCharsetWeightList::iterator iter;

	resetCreateTimes();
	//
	// make a CharWeight list copy for restoring later.
	//
	AosCharsetWeightList tmpList = mCharWeightList;
	for (u32 i=0; i<len; i++)
	{
		getTotalWeight();

		OmnNotImplementedYet;
	/*
	 * Trying to replace std::string to OmnString, but this code was broken.
	 * Chen Ding, 12/16/2008
		u32 weight = 1;
		u32 randValue = aos_next_integer<u32>(1, mTotalWeight);
		for (iter=mCharWeightList.begin(); iter!=mCharWeightList.end(); iter++)
		{
			if (iter->mWeight== 0)
			{
				continue;
			}
			if (weight <= randValue && randValue <= (weight+(*iter).mWeight))
			{
				//
				// generate a not ordered string, append a seperator, and set its weight to 0
				//
				destStr += iter->mChar;
				iter->mWeight = 0;
				iter->mCreateTimes = 1;
				break;
			}
			weight += iter->mWeight;
		}
		*/
	}

	//
	// restore the CharWeight list
	//
	mCharWeightList.swap(tmpList);
	return true;
}


// 
// Chen Ding
// Please add a description about how you generate multiple and
// ordered strings.
//
bool 
AosCharsetRSG::nextMultipleOrdered(OmnString &destStr, const u32 len)
{
	AosCharsetWeightList::iterator iter;

	getTotalWeight();
	resetCreateTimes();
	//
	// create chars randomly, set createTimes
	//
	for (u32 i=0; i<len; i++)
	{
		u32 weight = 1;
		u32 randValue = aos_next_integer<u32>(1, mTotalWeight);

		for (iter=mCharWeightList.begin(); iter!=mCharWeightList.end(); iter++)
		{
			if (iter->mWeight== 0)
			{
				continue;
			}
			if (weight <= randValue && randValue <= (weight+(*iter).mWeight))
			{
				iter->mCreateTimes ++;
				break;
			}
			weight += iter->mWeight;
		}
	}

	OmnNotImplementedYet;
	/*
	 * Trying to replace std::string to OmnString, but this code was broken.
	 * Chen Ding, 12/16/2008
	//
	// create ordered dest string
	//
	for (iter=mCharWeightList.begin(); iter!=mCharWeightList.end(); iter++)
	{
		for (u32 j=0; j<iter->mCreateTimes; j++)
		{
			destStr += iter->mChar;
		}
	}
	*/
	return true;
}


// 
// Please add a description about how you generate multiple and not ordered
// strings.
//
bool 
AosCharsetRSG::nextMultipleNotOrdered(OmnString &destStr, const u32 len)
{
	AosCharsetWeightList::iterator iter;

	getTotalWeight();

	// 
	// Chen Ding
	// This implementation is not efficient. We may use RIG to randomly
	// select the index. The RIG is actually a charset. 
	//
	// for (u32 i=0; i<len; i++)
	// {
	// 		int index = mCharset.nextInt(); 
	//		destStr += (char)index;
	// }
	//
	// where: 'mCharset' is an RIG. This will become clear when we work
	// on RIG. 
	//
	
/*	AosRIG<int8_t> rig;
	for (iter=mCharWeightList.begin(); iter!=mCharWeightList.end(); iter++)
	{
		rig.setIntegerPair(iter->mChar, iter->mChar, iter->mWeight);
	}
	for (u32 i=0; i<len; i++)
	{
		AosValue value;
		rig.nextValue(value);
		destStr += (char)value.toInt8();
	}
*/
	OmnNotImplementedYet;
	/*
	 * Trying to replace std::string to OmnString, but this code was broken.
	 * Chen Ding, 12/16/2008
	for (u32 i=0; i<len; i++)
	{
		u32 weight = 1;
		u32 randValue = aos_next_integer<u32>(1, mTotalWeight);

		for (iter=mCharWeightList.begin(); iter!=mCharWeightList.end(); iter++)
		{
			if (iter->mWeight== 0)
			{
				continue;
			}
			if (weight <= randValue && randValue <= (weight+(*iter).mWeight))
			{
				destStr += iter->mChar;
				break;
			}
			weight += iter->mWeight;
		}
	}
	*/
	return true;
}


u32 
AosCharsetRSG::getTotalWeight()
{
	mTotalWeight = 0;
	AosCharsetWeightList::iterator iter;
	for (iter=mCharWeightList.begin(); iter!=mCharWeightList.end(); iter++)
	{
		mTotalWeight += iter->mWeight;		
	}
	return mTotalWeight;
}


void 
AosCharsetRSG::resetCreateTimes()
{
	AosCharsetWeightList::iterator iter;
	for (iter=mCharWeightList.begin(); iter!=mCharWeightList.end(); iter++)
	{
		iter->mCreateTimes = 0;		
	}
}


bool 
AosCharsetRSG::clearCharset()
{
	mCharWeightList.clear();
	return true;
}

