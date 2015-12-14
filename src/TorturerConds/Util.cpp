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
// 05/14/2007: Created by Chen Ding
//
////////////////////////////////////////////////////////////////////////////
#include "TorturerConds/Util.h"

#include "Alarm/Alarm.h"
#include "Util/StrParser.h"



AosTortCondOpr AosTorCondOpr_toEnum(const OmnString &opr)
{
	OmnString p = opr;
	p.toLower();

	if (p == "<")
	{
		return eAosTortCondOpr_Less;
	}
	
	if (p == "<=")
	{
		return eAosTortCondOpr_LessEqual;
	}

	if (p == "==")
	{
		return eAosTortCondOpr_Equal;
	}

	if (p == ">=")
	{
		return eAosTortCondOpr_LargerEqual;
	}

	if (p == ">")
	{
		return eAosTortCondOpr_Larger;
	}

	if (p == ">=")
	{
		return eAosTortCondOpr_NotEqual;
	}

	return eAosTortCondOpr_Invalid;
}


int AosTimeStr_compare(const OmnString &v1, 
					   const OmnString &v2, 
					   OmnString &errmsg)
{
	// 
	// Time strings are expressed in:
	// 	"HH:MM:SS". 
	// where "HH" is expressed [0, 23]. 
	// If any of the two values are invalid, it returns -100. 
	//
	// Otherwise, it returns:
	// 	-1: v1 < v2
	// 	 0: v1 == v2
	// 	 1: v1 > v2
	//
	int hh1, mm1, ss1, hh2, mm2, ss2;
	if (!AosTimeStr_getValues(v1, hh1, mm1, ss1, errmsg)) return -100;
	if (!AosTimeStr_getValues(v2, hh2, mm2, ss2, errmsg)) return -100;

	if (hh1 < hh2) return -1;
	if (hh1 > hh2) return 1;
	if (mm1 < mm2) return -1;
	if (mm1 > mm2) return 1;
	if (ss1 < ss2) return -1;
	if (ss1 > ss2) return 1;
	return 0;
}


bool AosTimeStr_getValues(const OmnString &value, 
						 int &hh, 
						 int &mm, 
						 int &ss, 
						 OmnString &errmsg)
{
	OmnStrParser parser(value);
	OmnString str = parser.nextWord("", ':', true);
	hh = 0;
	mm = 0;
	ss = 0;
	if (str == "" || !str.isDigitStr()) 
	{
		errmsg << "Time string format incorrect: " << value;
		return false;
	}

	hh = atoi(str);
	if (hh < 0 || hh >23) 
	{
		errmsg << "Hour value is not in [0, 23]: " << errmsg << str;
		return false;
	}

	if (!parser.hasMore()) return true;
	str = parser.nextWord("", ':', true);
	if (str == "" || !str.isDigitStr()) 
	{
		errmsg << "Time string format incorrect: " << value;
		return false;
	}

	mm = atoi(str);
	if (mm < 0 || mm > 59) 
	{
		errmsg << "Minute value is not in [0, 59]: " << str;
		return false;
	}

	if (!parser.hasMore()) return true;
	str = parser.nextWord("", ':', true);
	if (str == "" || !str.isDigitStr()) 
	{
		errmsg << "Time string format incorrect: " << value;
		return false;
	}

	ss = atoi(str);
	if (ss < 0 || ss > 59) 
	{
		errmsg << "Second value is not in [0, 59]: " << str;
		return false;
	}

	return true;
}

	
AosOperandType 
AosOperandType_toEnum(const OmnString &opr)
{
	if (opr == "Parm") return eAosOperandType_Parm;
	if (opr == "Variable") return eAosOperandType_Var;
	if (opr == "CrtValue") return eAosOperandType_CrtValue;
	if (opr == "Constant") return eAosOperandType_Constant;

	return eAosOperandType_Invalid;
}


OmnString
AosOperandType_toStr(const AosOperandType type)
{
	switch (type)
	{
	case eAosOperandType_Parm:
		 return "Parm";

	case eAosOperandType_Var:
		 return "Variable";

	case eAosOperandType_CrtValue:
		 return "CrtValue";

	case eAosOperandType_Constant:
		 return "Constant";

	default:
		 OmnAlarm << "Unrecognized type: " << type << enderr;
		 return "Unknown";
	}

	return "Error";
}

