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
#include "Random/RandomSelector.h"

#include "Alarm/Alarm.h"
#include "Random/RandomUtil.h"
#include "Random/RandomInteger.h"
#include "XmlParser/XmlItem.h"



AosRandomSelector::AosRandomSelector()
:
AosRandomObj()
{
}


AosRandomSelector::AosRandomSelector(const OmnString &name)
:
AosRandomObj(name)
{
}


AosRandomSelector::~AosRandomSelector()
{
}


// 
// This is a static member function that creates RandomSelectors
// based on the configuration 'conf'. 
//
AosRandomSelectorPtr
AosRandomSelector::create(const OmnString &name, 
						  const OmnXmlItemPtr &conf) 
{
	// 
	// <Selector>
	//     <Type>type</Type>
	//     <Base>base</Base>
	//     <Elements>
	//         <Element>...
	//         <Element>...
	//         ...
	//         <Element>
	//     </Elements>
	// </Selector>
	//
	AosRandomSelectorPtr obj = 0;
	OmnString type = conf->getStr("Type", "");
	if (type == "")
	{
		OmnAlarm << "Missing type: " << conf->toString() << enderr;
		return 0;
	}

	int base = conf->getInt("Base", 100);
	if (type == "IntegerRandom")
	{
		obj = createRandomInteger(name, conf->getItem("Elements"), base);
	}
	else
	{
		OmnAlarm << "Unrecognized selector type: " 
			<< conf->toString() << enderr;
		return 0;
	}

	return obj;
}


AosRandomSelectorPtr
AosRandomSelector::createRandomInteger(
		const OmnString &name, 
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
		AosRandomIntegerPtr s = OmnNew AosRandomInteger(name, conf, base);
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
