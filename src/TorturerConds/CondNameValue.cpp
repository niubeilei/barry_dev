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
//  This class simulates the name-value condition. This class identifies
//  an argument in a command and a value. At run time, when a command
//  is genreated, its identified argument value should be the same
//  as the value stored in this class.
//
// Modification History:
// 05/15/2007: Created by Chen Ding
//
////////////////////////////////////////////////////////////////////////////
#include "TorturerConds/CondNameValue.h"

#include "Alarm/Alarm.h"
#include "aosUtil/Alarm.h"
#include "Debug/Debug.h"
#include "Tester/TestPkg.h"
#include "Tester/TestMgr.h"
#include "Parms/Util.h"
#include "TorturerConds/CondData.h"
#include "Util/GenTable.h"



AosTortCondNameValue::AosTortCondNameValue(
			const OmnString &name, 
			const OmnString &value, 
		    const AosTortCondOpr opr)
:
AosTortCond(opr),
mName(name),
mValue(value),
mType(AosRVGType::eUnknown)
{
}


AosTortCondNameValue::~AosTortCondNameValue()
{
}


bool	
AosTortCondNameValue::check(const AosCondData &data, 
						bool &rslt, 
						OmnString &errmsg) const
{
	// 
	// First, retrieve the argument value from the command. 
	//
	OmnString value;
	OmnString fieldname;
	if (!data.getVar(mName, value))
	{
		OmnAlarm << "Failed to retrieve the value: " 
			<< mName << enderr;
		errmsg = "Failed to retrieve the value";
		return false;
	}

	// 
	// Make sure the type matches.
	//
	switch (mOpr)
	{
	case eAosTortCondOpr_Equal:
		 if (value == mValue) return (rslt = true);
		 rslt = false;
		 (errmsg = mName) << " == " << mValue << " does not hold. "
		 	<< "Value stored: " << value
			<< ". Value expected: " << mValue;
		 return false;

	case eAosTortCondOpr_NotEqual:
		 if (value != mValue) return (rslt = true);
		 rslt = false;
		 (errmsg = mName) << " != " << mValue << " does not hold. "
		 	<< "Value stored: " << value
			<< ". Value expected: " << mValue;
		 return false;

	default:
		 OmnAlarm << "Other operators are not supported for SetVars" << enderr;
		 rslt = false;
		 errmsg = "Other operators are not supported for SetVars";
		 return false;
	}
	
//	if (type != mType)
//	{
//		OmnAlarm << "Type mismatch: " << type << ":" << mType << enderr;
//		errmsg = "Type mismatch: ";
//		errmsg << type << ":" << mType;
//		return false;
//	}
//
//	switch (type)
//	{
//	case eAosParmType_Int:
//		 {
//		 	int v1 = atoi(value.data());
//			int v2 = atoi(mValue.data());
//			bool ret = checkValues(v1, v2, mName, mValue, rslt, errmsg);
//			return ret;
//		 }

//	case eAosParmType_U32:
//		 {
//		 	u32 v1 = (u32)atoi(value.data());
//		 	u32 v2 = (u32)atoi(mValue.data());
//			return checkValues(v1, v2, mName, mValue, rslt, errmsg);
//		 }
//
//	case eAosParmType_Str:
//	case eAosParmType_StrEnum:
//	case eAosParmType_IP:
//	case eAosParmType_Netmask:
//	case eAosParmType_Mac:
//		 OmnAlarm << "Cannot compare two strings: " << enderr;
//		 errmsg = "Cannot compare";
//		 return false;
//
//	default:
//		 OmnAlarm << "Unrecognized parm type: " << type<< enderr;
//		 errmsg = "Unrecognized type";
//		 return false;
//	}

	errmsg = "Program error in namevar";
	rslt = false;
	return false;
}

