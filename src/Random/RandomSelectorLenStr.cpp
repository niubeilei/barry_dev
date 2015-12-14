////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: RandomSelector.h
// This class defines a super class for all RandomSelectors. A random 
// selector maintains a selection model, which is an array of weights
// and an array of items to be selected. 
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////
#include "Random/RandomSelectorLenStr.h"

#include "Alarm/Alarm.h"
#include "Random/RandomUtil.h"
#include "Random/RandomLenStr.h"
#include "XmlParser/XmlItem.h"



AosRandomSelectorLenStr::AosRandomSelectorLenStr()
:
AosRandomObj()
{
}


AosRandomSelectorLenStr::AosRandomSelectorLenStr(const OmnString &name)
:
AosRandomObj(name)
{
}


AosRandomSelectorLenStr::~AosRandomSelectorLenStr()
{
}


// 
// This is a static member function that creates RandomSelectors
// based on the configuration 'conf'. 
//
AosRandomSelectorLenStrPtr
AosRandomSelectorLenStr::create(
			const OmnString &name, 
			const OmnXmlItemPtr &conf) 
{
	// 
	// <Selector>
	//     <Type>type</Type>
	//     <StrType>type</StrType>
	//     <Base>base</Base>
	//     <Elements>
	//         <Element>...
	//         <Element>...
	//         ...
	//         <Element>
	//     </Elements>
	// </Selector>
	//
	AosRandomSelectorLenStrPtr obj = 0;
	OmnString type = conf->getStr("Type", "");
	OmnString strType = conf->getStr("StrType", "");
	int base = conf->getInt("Base", 100);
	int strTypeEnum = -1;
	if (strType == "")
	{
		OmnAlarm << "Missing type: " << conf->toString() << enderr;
		strTypeEnum = -1;
	}
	else if (strType == "Str")
	{
		strTypeEnum = AosRandomLenStr::eStr;
	}
	else if (strType == "AsciiStr")
	{
		strTypeEnum = AosRandomLenStr::eAsciiStr;
	}
	else if (strType == "BinStr")
	{
		strTypeEnum = AosRandomLenStr::eBinStr;
	}
	else if (strType == "LowcaseStr")
	{
		strTypeEnum = AosRandomLenStr::eLowcaseStr;
	}
	else if (strType == "CapitalStr")
	{
		strTypeEnum = AosRandomLenStr::eCapitalStr;
	}
	else if (strType == "DigitStr")
	{
		strTypeEnum = AosRandomLenStr::eDigitStr;
	}
	else if (strType == "LetterStr")
	{
		strTypeEnum = AosRandomLenStr::eLetterStr;
	}
	else if (strType == "LetterDigitStr")
	{
		strTypeEnum = AosRandomLenStr::eLetterDigitStr;
	}
	else if (strType == "VarNameStr")
	{
		strTypeEnum = AosRandomLenStr::eVarNameStr;
	}
	else
	{
		OmnAlarm << "Unrecognized string type: " << conf->toString() << enderr;
		strTypeEnum = -1;
	}

	if (type == "")
	{
		OmnAlarm << "Missing type: " << conf->toString() << enderr;
		return 0;
	}

	if (type == "RandomLenStr")
	{
		obj = createRandomLenStr(name, strTypeEnum, conf->getItem("Elements"), base);
	}
	else
	{
		OmnAlarm << "Unrecognized selector type: " << conf->toString() << enderr;
		return 0;
	}

	return obj;
}


AosRandomSelectorLenStrPtr
AosRandomSelectorLenStr::createRandomLenStr(
				const OmnString &name, 
				const int strTypeEnum, 
				const OmnXmlItemPtr &conf, 
				const u32 base)
{
	// 
	// <Elements>
	//     <Element>...
	//     <Element>...
	//     ...
	// </Elements>
	//
	if (!conf)
	{
		OmnAlarm << "Missing Elements section" << enderr;
		return 0;
	}

	try
	{
		AosRandomLenStrPtr s = OmnNew AosRandomLenStr(name, strTypeEnum, conf, base);
		if (s && s->isGood())
		{
			return s;
		}

		// It is not a good one. Return NULL
		return 0;
	}

	catch (const OmnExcept &e)
	{
		OmnAlarm << "Failed to create selector: " 
			<< e.toString() << enderr;
		return 0;
	}
}
