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
// 	This is a special type of global VarWithMap that has a map that 
// 	maps input data from one set to another set.
// 	As an example, we may define a VarWithMap whose name is:
// 	"on" and "off"
// 	but the program will use integer values: 0 and 1. 
//
// Modification History:
// 07/27/2007: Created by Chen Ding
//
////////////////////////////////////////////////////////////////////////////
#include "TorturerUtil/VarWithMap.h"

#include "Alarm/Alarm.h"
#include "Util/OmnNew.h"
#include "Util/StrParser.h"
#include "XmlParser/XmlItem.h"


AosVarWithMap::AosVarWithMap()
{
}


AosVarWithMap::~AosVarWithMap()
{
}


bool
AosVarWithMap::config(const OmnXmlItemPtr &def)
{
	// 
	// 	<GlobalWithMap>
	// 		<Name>
	// 		<Value>
	// 		<Map>[v1, v2], [v1, v2], ...</From>
	// 		<GetFlag>from|to</GetFlag>
	// 	</GlobalWithMap>	
	// 
	if (!AosVariable::config(def)) return false;
	OmnString map = def->getStr("Map", "");
	OmnStrParser parser(map);
	parser.reset();
	OmnString from, to;
	while (parser.hasMore())
	{   
		if (!parser.getValuePair(from, to, '[', ','))
		{   
			OmnAlarm << "Invalid pair: " << map << enderr;
			return false;
		}

		mFrom.append(from);
		mTo.append(to);
	}

	OmnString getFlag = def->getStr("GetFlag", "");
	if (getFlag == "from")
	{
		mGetFlag = eGetFlag_From;
	}
	else if (getFlag == "to")
	{
		mGetFlag = eGetFlag_To;
	}
	else
	{
		OmnAlarm << "Invalid GetFlag: " << def->toString() << enderr;
		return false;
	}

	return true;
}


AosVarWithMapPtr
AosVarWithMap::createVarWithMap(const OmnXmlItemPtr &def)
{
	AosVarWithMapPtr var = OmnNew AosVarWithMap();
	if (!var->config(def)) return 0;

	return var;
}


OmnString
AosVarWithMap::getValue() const
{
	if (mGetFlag == eGetFlag_To)
	{
		return mValue;
	}

	for (int i=0; i<mTo.entries(); i++)
	{
		if (mTo[i] == mValue) return mFrom[i];
	}

	OmnAlarm << "Did not find the value: " << mValue << enderr;
	return "";
}

