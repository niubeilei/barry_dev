////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: RandomObj.cpp
// Refer to the comments in RandomObj.h
//
// Modification History:
// 11/22/2006 Created by Chen Ding
//
////////////////////////////////////////////////////////////////////////////
#include "Random/RandomObj.h"

#include "Random/RandomSelector.h"
#include "Random/RandomSelStr.h"
#include "Random/RandomSelectorLenStr.h"
#include "XmlParser/XmlItem.h"


AosRandomObj::AosRandomObj()
:
mIsGood(false)
{
}


AosRandomObj::AosRandomObj(const OmnString &name)
:
mName(name),
mIsGood(false)
{
}


AosRandomObj::~AosRandomObj()
{
}


AosRandomObjPtr
AosRandomObj::createRandomObj(const OmnString &conf)
{
	OmnXmlItemPtr c = OmnNew OmnXmlItem(conf);
	return createRandomObj(c);
}


AosRandomObjPtr
AosRandomObj::createRandomObj(const OmnXmlItemPtr &conf)
{
	// 
	// It assumes the following format:
	// <Random>
	//     <Name>
	//     <Type>
	//     <Selector>
	//         ...
	//     </Selector>
	// </Random>
	//
	// The <Variables> section is optional. This section defines random
	// variables that may be needed by the <Selector>
	//
	
	OmnString name;
	RandomObjType type;
	AosRandomObjPtr robj;

	if (!retrieveName(conf, name)) return 0;
	if (!retrieveType(conf, type)) return 0;

	switch (type)
	{
	case eSelector:
		 robj = AosRandomSelector::create(name, conf->getItem("Selector"));
		 break;

	case eIntegerSelector:
		 robj = AosRandomSelector::create(name, conf->getItem("Selector"));
		 break;

	case eStringSelector:
		 robj = OmnNew AosRandomSelStr(name, conf->getItem("Selector"));
		 break;

	case eLenStringSelector:
		 robj = AosRandomSelectorLenStr::create(name, conf->getItem("Selector"));
		 break;


	default:
		 OmnAlarm << "Unrecognized random type: " << type 
			 << ". " << conf->toString() << enderr;
		 return 0;
	}

	if (!robj)
	{
		OmnAlarm << "Failed to create obj: " << conf->toString() << enderr;
		return 0;
	}

//	OmnVList<AosRandomObjPtr> vars;
//	if (!createVariables(conf, vars)) return false;
//	if (!robj->setVariables(vars)) return false;

	return robj;
}


bool
AosRandomObj::retrieveName(const OmnXmlItemPtr &conf, OmnString &name)
{
	name = conf->getStr("Name", "");
	if (name.length() == 0)
	{
		OmnAlarm << "Missing Random Name: " << conf->toString() << enderr;
		return false;
	}

	return true;
}


bool
AosRandomObj::retrieveType(const OmnXmlItemPtr &conf, RandomObjType &type)
{
	OmnString t = conf->getStr("Type", "");
	if (t.length() == 0)
	{
		OmnAlarm << "Missing Random Type: " << conf->toString() << enderr;
		return false;
	}

	if (t == "Selector")
	{
		type = eSelector;
	}
	else if (t == "StringSelector")
	{
		type = eStringSelector;
	}
	else if (t == "LenStringSelector")
	{
		type = eLenStringSelector;
	}
	else
	{
		OmnAlarm << "Unrecognized random type: " << conf->toString() << enderr;
		return false;
	}

	return true;
}


bool
AosRandomObj::expect(OmnStrParser1 &parser, char c, const OmnString &contents, const OmnXmlItemPtr &conf)
{
	if (!parser.expectNext(c, true))
	{
		OmnAlarm << "Expecting: " << c 
			<< " of: " << contents << " in: " << conf->toString() << enderr;
		return false;
	}

	return true;
}


bool
AosRandomObj::getInt(
		   	OmnStrParser1 &parser, 
			int64_t &value, 
			const OmnString &message, 
			const OmnString &contents, 
			const OmnXmlItemPtr &conf)
{
	if (!parser.nextInt(value, -1))
	{
		OmnAlarm << "Failed to retrieve: " 
			<< message << " of: " << contents << " in: " 
			<< conf->toString() << enderr;
		return false;
	}

	return true;
}


bool
AosRandomObj::getStr(
		   	OmnStrParser1 &parser, 
			OmnString &value, 
			const OmnString &message, 
			const OmnString &contents, 
			const OmnXmlItemPtr &conf)
{
	value = parser.nextWord("");
	if (value.length() <= 0)
	{
		OmnAlarm << "Failed to retrieve: " 
			<< message << " of: " << contents << " in: " << conf->toString() << enderr;
		return false;
	}

	return true;
}


bool
AosRandomObj::finalCheck(const OmnXmlItemPtr &conf, const int weightTotal)
{
	if (mBase == 0)
	{
		return true;
	}

	if ((u32)weightTotal != mBase)
	{
		OmnAlarm << "Weights total is not mBase: " 
			<< mBase << ". " 
			<< conf->toString() << enderr;
		return false;
	}

	if (conf->hasMore())
	{
		OmnAlarm << "Config is not correct: " << conf->toString() << enderr;
		return false;
	}

	return true;
}

OmnString	
AosRandomObj::nextIncorrectStr(AosRVGReturnCode &rcode, OmnString &errmsg)
{
	// 
	// this function should never be called. 
	//
	(errmsg = "Program error: ") << __FILE__ << ":" << __LINE__;
	OmnAlarm << errmsg << enderr;
	rcode = eAosRVGRc_ProgramError;
	return "******BAD******";
}


bool
AosRandomObj::checkWeight(int weight, const OmnXmlItemPtr &conf)
{
	if (weight < 0)
	{
		OmnWarn << "The weight: " << weight 
			<< " is negative: " << weight << enderr;
		return false;
	}

	if (mBase == 0)
	{
		return true;
	}

	if ((u32)weight > mBase)
	{
        OmnWarn << "The weight: " << weight 
			<< " is too big: " 
			<< weight << ". Maximum: " << mBase << enderr;;
        return false;
    }

	return true; 
}
