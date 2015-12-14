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
// A RandomLenStr is a random object that generates strings randomly.
// The generate strings' length is determined by an integer selector
// (an instance of AosRandomInteger):
//     [low1, high1], weight1
//     [low2, high2], weight2
//     [low3, high3], weight3
//     ...
//     [lown, highn], weightn
//
// The object can be defined by an XML document:
// <Random>
//     <Name>
//     <Type>Selector</Type>
//     <Selector>
//         <Type>RandomLenStr</Type>
//         <StrType>BinStr</StrType>
//         <Base>base</Base>
//         <Elements>
//             <Element>value, weight</Element>
//             <Element>[low, high], weight</Element>
//             ...
//             <Element>[low, high], weight</Element>
//         </Elements>
//     </Selector>
// </Random>
//
// Modification History:
// 12/04/2006 Created by Chen Ding
//
////////////////////////////////////////////////////////////////////////////
#include "Random/RandomLenStr.h"

#include "Alarm/Alarm.h"
#include "Random/RandomInteger.h"
#include "Random/RandomUtil.h"


AosRandomLenStr::AosRandomLenStr()
	:
mStrType(0)
{
}


AosRandomLenStr::AosRandomLenStr(
		const OmnString &name, 
		const int strTypeEnum,
		const OmnXmlItemPtr &conf, 
		const u32 base)
:
AosRandomSelectorLenStr(name),
mStrType(strTypeEnum)
{
	// This constructor creates the instance based on an XML configuration:
	// <Random>
	//     <Name>
	//     <Type>Selector</Type>
	//     <Selector>
	//         <Type>LenStrSelector</Type>
	//         <StrType>BinStr</StrType>
	//         <Base>base</Base>
	//         <Elements>
	//             <Element>value, weight</Element>
	//             <Element>[low, high], weight</Element>
	//             ...
	//             <Element>
	//         </Elements>
	//     </Selector>
	// </Random>
	// 
	// When this function is called, 'conf' is:
	//         <Elements>
	//             <Element>
	//             <Element>
	//             ...
	//             <Element>
	//         </Elements>
	//
	mLenSelector = OmnNew AosRandomInteger(name, conf, base);
	mIsGood = mLenSelector->isGood();
}


AosRandomLenStr::~AosRandomLenStr()
{
}


// 
// This function selects an index based on the weights
//
int 		
AosRandomLenStr::nextInt(AosGenTable &data)
{
	OmnAlarm << "Cannot call AosRandomLenStr::nextInt()" << enderr;
	return -1;
}

int 		
AosRandomLenStr::nextInt()
{
	OmnAlarm << "Cannot call AosRandomLenStr::nextInt()" << enderr;
	return -1;
}


int 		
AosRandomLenStr::nextInt(const OmnString &name)
{
	OmnAlarm << "Cannot call AosRandomLenStr::nextInt()" << enderr;
	return -1;
}


int 		
AosRandomLenStr::nextInt(const OmnString &name, AosGenTable &data)
{
	OmnAlarm << "Cannot call AosRandomLenStr::nextInt()" << enderr;
	return -1;
}


OmnString
AosRandomLenStr::nextStr(AosGenTable &data)
{
	return nextStr();
}


OmnString
AosRandomLenStr::nextStr()
{
	if (!mLenSelector)
	{
		OmnAlarm << "Length selector not initialized yet" << enderr;
		return "";
	}

	int len = mLenSelector->nextInt();

	if (mStrType==eStr) 
	{
		return OmnRandom::printableStr(len, len, false);
	}

	if (mStrType == eAsciiStr) 
	{
		return OmnRandom::asciiStr(len, len);
	}
	
	if (mStrType==eBinStr) 
	{
		return OmnRandom::binaryStr(len, len);
	}

	if (mStrType == eLowcaseStr) 
	{
		return OmnRandom::lowcaseStr(len, len);
	}

	if (mStrType == eCapitalStr) 
	{
		return OmnRandom::capitalStr(len, len);
	}
	
	if (mStrType == eDigitStr) 
	{
		return OmnRandom::digitStr(len, len);
	}

	if (mStrType == eLetterStr) 
	{
		return OmnRandom::letterStr(len, len);
	}

	if (mStrType == eLetterDigitStr) 
	{
		return OmnRandom::letterDigitStr(len, len, false);
	}

	if (mStrType == eVarNameStr) 
	{
		return OmnRandom::varName(len, len, true);
	}
	
	OmnAlarm << "Not right string type." << enderr;
	return "";
}


OmnString
AosRandomLenStr::nextStr(const OmnString &name, AosGenTable &data)
{
	return nextStr();
}


OmnString
AosRandomLenStr::nextStr(const OmnString &name)
{
	return nextStr();
}


int         
AosRandomLenStr::getNumElems() const 
{
	if (!mLenSelector)
	{
		OmnAlarm << "Len selector is null" << enderr;
		return -1;
	}

	return mLenSelector->getNumElems();
}

