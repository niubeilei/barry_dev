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
// 07/18/2007: Created by Chen Ding
//
////////////////////////////////////////////////////////////////////////////
#include "LogicExpr/LogicExprUtil.h"

#include "aosUtil/Alarm.h"
#include "Alarm/Alarm.h"

bool AosEvaluate(const AosDataType type, 
				 const OmnString &value1, 
				 const OmnString &value2, 
				 const AosTortCondOpr opr,
				 bool &rslt, 
				 OmnString &errmsg)
{
	switch (type)
	{
	case eAosDataType_Integral:
		 return AosEvaluateInt(value1, value2, opr, rslt, errmsg);

	case eAosDataType_Unsigned:
		 return AosEvaluateUnsigned(value1, value2, opr, rslt, errmsg);

	case eAosDataType_String:
		 return AosEvaluateString(value1, value2, opr, rslt, errmsg);

	default:
		 errmsg << "Unrecognized data type: " << type;
		 OmnAlarm << errmsg << enderr;
		 rslt = false;
		 return false;
	}

	return false;
}


bool AosEvaluateInt(
				 const OmnString &value1, 
				 const OmnString &value2, 
				 const AosTortCondOpr opr,
				 bool &rslt, 
				 OmnString &errmsg)
{
	int64_t v1 = atoll(value1);
	int64_t v2 = atoll(value2);

	switch (opr)
	{
	case eAosTortCondOpr_Less:
		 rslt = (v1 < v2);
		 return true;

	case eAosTortCondOpr_LessEqual:
		 rslt = (v1 <= v2);
		 return true;

	case eAosTortCondOpr_Equal:
		 rslt = (v1 == v2);
		 return true;

	case eAosTortCondOpr_LargerEqual:
		 rslt = (v1 >= v2);
		 return true;

	case eAosTortCondOpr_Larger:
		 rslt = (v1 > v2);
		 return true;

	case eAosTortCondOpr_NotEqual:
		 rslt = (v1 != v2);
		 return true;

	default:
		 errmsg << "Program Error. Unrecognized operator: " << opr;
		 OmnAlarm << errmsg << enderr;
		 return false;
	}

	return false;
}


bool AosEvaluateUnsigned(
				 const OmnString &value1, 
				 const OmnString &value2, 
				 const AosTortCondOpr opr,
				 bool &rslt, 
				 OmnString &errmsg)
{
	u64 v1 = (u64)atoll(value1);
	u64 v2 = (u64)atoll(value2);

	switch (opr)
	{
	case eAosTortCondOpr_Less:
		 rslt = (v1 < v2);
		 return true;

	case eAosTortCondOpr_LessEqual:
		 rslt = (v1 <= v2);
		 return true;

	case eAosTortCondOpr_Equal:
		 rslt = (v1 == v2);
		 return true;

	case eAosTortCondOpr_LargerEqual:
		 rslt = (v1 >= v2);
		 return true;

	case eAosTortCondOpr_Larger:
		 rslt = (v1 > v2);
		 return true;

	case eAosTortCondOpr_NotEqual:
		 rslt = (v1 != v2);
		 return true;

	default:
		 errmsg << "Program Error. Unrecognized operator: " << opr;
		 OmnAlarm << errmsg << enderr;
		 return false;
	}

	return false;
}

bool AosEvaluateString(
				 const OmnString &value1, 
				 const OmnString &value2, 
				 const AosTortCondOpr opr,
				 bool &rslt, 
				 OmnString &errmsg)
{
	switch (opr)
	{
	case eAosTortCondOpr_Less:
		 rslt = (strcmp(value1, value2) < 0);
		 return true;

	case eAosTortCondOpr_LessEqual:
		 rslt = (strcmp(value1, value2) <= 0);
		 return true;

	case eAosTortCondOpr_Equal:
		 rslt = (strcmp(value1, value2) == 0);
		 return true;

	case eAosTortCondOpr_LargerEqual:
		 rslt = (strcmp(value1, value2) >= 0);
		 return true;

	case eAosTortCondOpr_Larger:
		 rslt = (strcmp(value1, value2) > 0);
		 return true;

	case eAosTortCondOpr_NotEqual:
		 rslt = (strcmp(value1, value2) != 0);
		 return true;

	default:
		 errmsg << "Program Error. Unrecognized operator: " << opr;
		 OmnAlarm << errmsg << enderr;
		 return false;
	}

	return false;
}


AosDataType 	
AosDataType_toEnum(const OmnString &name)
{
	OmnString nn = name;
	nn.toLower();
	if (name == "integer") return eAosDataType_Integral;
	if (name == "unsigned") return eAosDataType_Unsigned;
	if (name == "string") return eAosDataType_String;

	OmnAlarm << "Unrecognized data type: " << name << enderr;
	return eAosDataType_Invalid;
}



