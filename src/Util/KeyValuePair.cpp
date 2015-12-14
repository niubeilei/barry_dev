////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
//
// Modification History:
// 2013/07/25 Created by Young Pan
////////////////////////////////////////////////////////////////////////////

#include "Util/KeyValuePair.h"

#include "alarm_c/alarm.h"
#include "Debug/Debug.h"
#include "Alarm/Alarm.h"
#include "Util/Ptrs.h"


AosKeyValuePair::AosKeyValuePair(
		AosRundata *rdata,
		const OmnString &contents)
:mStrLine("")
{
	createPairs(rdata, contents);
}


AosKeyValuePair::AosKeyValuePair(OmnString pline)
:
mStrLine(pline)
{
}


AosKeyValuePair::~AosKeyValuePair()
{
}


bool
AosKeyValuePair::createPairs(
		AosRundata *rdata, 
		const OmnString &contents)
{
	if (contents == "")
	{
		return true;
	}

	// 1. get lines
	OmnString pLine;
	OmnString str = contents;
	while((pLine = str.getLine(true, 1)) != "")
	{
		OmnString key = getKey(pLine);
		if (key == "") break;
		OmnString value = getValue(pLine);
		// 2. put key and value into map
		mKVMap.insert(std::pair<OmnString, OmnString>(key, value));
	}
	
	return true;
}


OmnString 
AosKeyValuePair::getKey(OmnString pline)
{
	aos_assert_r(pline != "", "");

	int pos = pline.find(':', false);
	aos_assert_r(pos >= 0, "");

	OmnString key = pline.subString(0, pos); 
	aos_assert_r(key != "", "");

	// trim the key 
	OmnString newkey = replaceStrWithSpace(key);
	newkey.toLower();

	return newkey;
}


OmnString 
AosKeyValuePair::replaceStrWithSpace(OmnString str)
{
	OmnString newStr;
	str.removeWhiteSpaces();
	int count = 0;
	for (int i=0; i<str.length(); i++)
	{
		if (str[i] == ' '  || 
			str[i] == '\n' ||
			str[i] == '\t' ||
			str[i] == '\r')  
		{           
			if (count >= 1) continue;
			count++;
			newStr << " ";
		}                              
		else                           
		{                              
			count = 0;
			newStr << str[i]; 
		}                              
	}
	
	return newStr;
}


OmnString 
AosKeyValuePair::getValue(OmnString pline)
{
	if (pline == "") return "";

	int pos = pline.find(':', false);
	aos_assert_r(pos >= 0, "");
	OmnString value = pline.substr(pos+1, pline.length()-1); 

	return value;
}

u32 		
AosKeyValuePair::getAttrU32(const OmnString &key, const u64 &dft)
{
	aos_assert_r(key != "", dft);
	map<OmnString, OmnString>::iterator itr = mKVMap.find(key);
	if(itr == mKVMap.end())
	{
		return dft;
	}
	
	OmnString value = itr->second;
	if (value == "")
	{
		return dft;
	}
	//aos_assert_r(value != "", dft);

	return value.parseU32(0);
}


u64 		
AosKeyValuePair::getAttrU64(const OmnString &key, const u64 &dft)
{
	aos_assert_r(key != "", dft);
	map<OmnString, OmnString>::iterator itr = mKVMap.find(key);
	if(itr == mKVMap.end())
	{
		return dft;
	}
	
	OmnString value = itr->second;
	if (value == "")
	{
		return dft;
	}

	return value.parseU64(0);
}


OmnString	
AosKeyValuePair::getAttrStr(const OmnString &key, const OmnString &dft)
{
	aos_assert_r(key != "", dft);
	map<OmnString, OmnString>::iterator itr = mKVMap.find(key);
	if(itr == mKVMap.end())
	{
		return dft;
	}
	
	OmnString value = itr->second;

	return value;
}


