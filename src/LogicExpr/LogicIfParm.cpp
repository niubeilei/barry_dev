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
// 07/16/2007: Created by Chen Ding
//
////////////////////////////////////////////////////////////////////////////
#include "LogicExpr/LogicIfParm.h"

#include "alarm/Alarm.h"
#include "Alarm/Alarm.h"
#include "LogicExpr/LogicObj.h"
#include "Util/StrParser.h"
#include "XmlParser/XmlItem.h"

AosLogicIfParm::AosLogicIfParm()
{
}


AosLogicIfParm::~AosLogicIfParm()
{
}


bool 	
AosLogicIfParm::config(const OmnXmlItemPtr &def)
{
	// 
	// 	<IfParm>
	// 		<if>parm(name) in {xx, xx, ...}</if>
	// 		<then>parm(name) in {xx, xx, ...}</then>
	// 	</IfParm>
	//
	aos_assert_r(def, false);

	OmnString ifstr = def->getStr("if", "");
	if (ifstr == "")
	{
		OmnAlarm << "Missing the 'if' tag: " << def->toString() << enderr;
		return false;
	}

	OmnStrParser parser(ifstr);
	mDependedName = parser.getSubstr("parm($$$)", "");
	if (mDependedName == "")
	{
		OmnAlarm << "Did not find 'parm(name)' portion in: " << ifstr
			<< ". The def: " << def->toString() << enderr;
		return false;
	}

//	if (!parser.getStrList("in {$$$}", mValues))
//	{
//		OmnAlarm << "Failed to get the values: " << ifstr
//			<< ". The def: " << def->toString() << enderr;
//		return false;
//	}

	return true;
}


bool
AosLogicIfParm::evaluate(bool &rslt, 
						 OmnString &errmsg,
						 const AosLogicObjPtr &obj) const
{
	aos_assert_r(obj, false);
	OmnString value;
    if (!obj->getValue(eAosOperandType_Parm, mDependedName, value, errmsg))
	{
		return false;
	}

	for (int i=0; i<mValues.entries(); i++)
	{
		if (value == mValues[i]) 
		{
			rslt = true;
			return true;
		}
	}

	rslt = false;
	return false;
}

