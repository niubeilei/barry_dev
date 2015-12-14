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
// mDataTypeName: The member data type name. 
// mDataName:     The member data name (not including the leading 'm')
//
// Modification History:
// 3/19/2007: Created by cding
////////////////////////////////////////////////////////////////////////////
#include "ProgramAid/Field.h"

#include "Alarm/Alarm.h"
#include "ProgramAid/Ptrs.h"


AosField::AosField()
{
}


AosField::AosField(const OmnString &fieldname, 
			const OmnString &tname, 
			const OmnString &dataname, 
			const OmnString &keyFlag, 
			const OmnString &dflt)
:
mFieldName(fieldname),
mDataTypeName(tname),
mDataName(dataname),
mDefaultValue(dflt),
mIsPrimaryKey(false)
{
	if (keyFlag == "yes")
	{
		mIsPrimaryKey = true;
	}

	if (mDataTypeName == "char")
	{
		mDefaultValue = "'";
		mDefaultValue << dflt << "'";
	}
}


AosField::~AosField()
{
}


OmnString
AosField::getMemDataName() const
{
	OmnString name = "m";
	name << mDataName;
	return name;
}


OmnString
AosField::getMemDataName(const bool capital) const
{
	OmnString name = mDataName;
	char *data = (char *)name.data();
	if (capital)
	{
		// 
		// The initial character needs to be capital
		//
		if (data[0] >= 'a' && data[0] <= 'z')
		{
			data[0] -= 32;
		}
	}
	else
	{
		// 
		// The initial character needs to be lower case
		//
		if (data[0] >= 'A' && data[0] <= 'Z')
		{
			data[0] += 32;
		}
	}

	return name;
}


OmnString
AosField::getHeader() const
{
	if (mDataTypeName == "OmnString")
	{
		return "#include \"Util/String.h\"";
	}

	return "";
}


OmnString
AosField::getDefaultValue() const
{
	if (mDefaultValue != "")
	{
		return mDefaultValue;
	}

	if (mDataTypeName == "long") return "0";
	if (mDataTypeName == "unsigned long") return "0";
	if (mDataTypeName == "int") return "0";
	if (mDataTypeName == "unsigned int") return "0";
	if (mDataTypeName == "u8") return "0";
	if (mDataTypeName == "u16") return "0";
	if (mDataTypeName == "u32") return "0";
	if (mDataTypeName == "short") return "0";
	if (mDataTypeName == "unsigned short") return "0";
	if (mDataTypeName == "char") return "'0'";
	if (mDataTypeName == "unsigned char") return "0";
	if (mDataTypeName == "bool") return "false";
	if (mDataTypeName == "float") return "0.0";
	if (mDataTypeName == "double") return "0.0";
	if (mDataTypeName == "OmnString") return "\"\"";

	OmnAlarm << "Unknown data type: " << mDataTypeName << enderr;
	return "\"\"";
}


bool
AosField::isStrType() const
{
	if (mDataTypeName == "OmnString" ||
		mDataTypeName == "char *")
	{
		return true;
	}

	return false;
}


bool
AosField::isIntegralType() const
{
	if (mDataTypeName == "long" || 
		mDataTypeName == "unsigned long" || 
		mDataTypeName == "int" || 
		mDataTypeName == "unsigned int" || 
		mDataTypeName == "u8" || 
		mDataTypeName == "u16" || 
		mDataTypeName == "u32" || 
		mDataTypeName == "short" || 
		mDataTypeName == "unsigned short" || 
		mDataTypeName == "char" || 
		mDataTypeName == "unsigned char" || 
		mDataTypeName == "bool")
	{
		return true;
	}

	return false;
}


bool
AosField::isFloatType() const 
{
	if (mDataTypeName == "float" || mDataTypeName == "double") return true;
	return false;
}


OmnString
AosField::getSingleQuote() const
{
	if (isStrType()) return "'";
	return "";
}


OmnString
AosField::getRecordTypeName() const
{
	if (mDataTypeName == "long") return "Int";
	if (mDataTypeName == "unsigned long") return "U32";
	if (mDataTypeName == "int") return "Int";
	if (mDataTypeName == "unsigned int") return "U32";
	if (mDataTypeName == "u8") return "U32";
	if (mDataTypeName == "u16") return "U32";
	if (mDataTypeName == "u32") return "U32";
	if (mDataTypeName == "short") return "Int";
	if (mDataTypeName == "unsigned short") return "U32";
	if (mDataTypeName == "char") return "Char";
	if (mDataTypeName == "unsigned char") return "Char";
	if (mDataTypeName == "bool") return "Bool";
	if (mDataTypeName == "float") return "Double";
	if (mDataTypeName == "double") return "Double";
	if (mDataTypeName == "OmnString") return "Str";

	OmnAlarm << "Unrecognized type name: " << mDataTypeName << enderr;
	return "Unknown";
}

