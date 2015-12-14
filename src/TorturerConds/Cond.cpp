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
#include "TorturerConds/Cond.h"

#include "Alarm/Alarm.h"
#include "Debug/Debug.h"
#include "TorturerConds/CondData.h"
#include "TorturerConds/CondArith.h"
#include "TorturerConds/CondNameValue.h"
#include "TorturerConds/CondReturnValue.h"
#include "Util/StrParser.h"
#include "XmlParser/XmlItem.h"



AosTortCond::AosTortCond()
{
}


AosTortCond::AosTortCond(const AosTortCondOpr opr)
:
mOpr(opr)
{
}


AosTortCond::~AosTortCond()
{
}


bool
AosTortCond::checkValues(const int v1, const int v2, 
					const OmnString &str1, 
					const OmnString &str2,
					bool &rslt, 
					OmnString &errmsg) const
{
	switch (mOpr)
	{
	case eAosTortCondOpr_Less:
		 if (v1 < v2) return rslt = true;
		 errmsg = str1; 
		 errmsg << " < " << str2 << " does not hold. ";
		 return rslt = false;

	case eAosTortCondOpr_LessEqual:
		 if (v1 <= v2) return rslt = true;
		 errmsg = str1; 
		 errmsg << " <= " << str2 << " does not hold. ";
		 return rslt = false;

	case eAosTortCondOpr_Equal:
		 if (v1 == v2) return rslt = true;
		 errmsg = str1; 
		 errmsg << " == " << str2 << " does not hold. ";
		 return rslt = false;

	case eAosTortCondOpr_LargerEqual:
		 if (v1 >= v2) return rslt = true;
		 errmsg = str1; 
		 errmsg << " >= " << str2 << " does not hold. ";
		 return rslt = false;

	case eAosTortCondOpr_Larger:
		 if (v1 > v2) return rslt = true;
		 errmsg = str1; 
		 errmsg << " > " << str2 << " does not hold. ";
		 return rslt = false;

	case eAosTortCondOpr_NotEqual:
		 if (v1 != v2) return rslt = true;
		 errmsg = str1; 
		 errmsg << " != " << str2 << " does not hold. ";
		 return rslt = false;

	default:
		 OmnAlarm << "Unrecognized operator: " << mOpr << enderr;
		 errmsg << "Unrecognized operator: " << mOpr;
		 return rslt = false;
	}

	return false;
}


bool
AosTortCond::checkValues(const u32 v1, const u32 v2, 
					const OmnString &str1, 
					const OmnString &str2, 
					bool &rslt, 
					OmnString &errmsg) const
{
	errmsg = "";
	switch (mOpr)
	{
	case eAosTortCondOpr_Less:
		 if (v1 < v2) return rslt = true;
		 errmsg << str1 << " < " << str2 << " does not hold. ";
		 return rslt = false;

	case eAosTortCondOpr_LessEqual:
		 if (v1 <= v2) return rslt = true;
		 errmsg << str1 << " <= " << str2 << " does not hold. ";
		 return rslt = false;

	case eAosTortCondOpr_Equal:
		 if (v1 == v2) return rslt = true;
		 errmsg << str1 << " == " << str2 << " does not hold. ";
		 return rslt = false;

	case eAosTortCondOpr_LargerEqual:
		 if (v1 >= v2) return rslt = true;
		 errmsg << str1 << " >= " << str2 << " does not hold. ";
		 return rslt = false;

	case eAosTortCondOpr_Larger:
		 if (v1 > v2) return rslt = true;
		 errmsg << str1 << " > " << str2 << " does not hold. ";
		 return rslt = false;

	case eAosTortCondOpr_NotEqual:
		 if (v1 != v2) return rslt = true;
		 errmsg << str1 << " != " << str2 << " does not hold. ";
		 return rslt = false;

	default:
		 OmnAlarm << "Unrecognized operator: " << mOpr << enderr;
		 errmsg << "Unrecognized operator: " << mOpr;
		 return rslt = false;
	}

	return rslt = false;
}


// 
// This function parses the conditions 'def' and create 
// them into the array 'conds'. 
//
bool
AosTortCond::parseConditions(const OmnXmlItemPtr &def, 
			OmnDynArray<AosTortCondPtr> &conds)
{
	// 
	// 	<Conditions>
	// 		<ArithCond>field1 opr field2</ArithCond>
	// 		<NameValueCond>varname opr value</NameValueCond>
	// 		<ReturnCheck1>
	// 		...
	// 	</Conditions>
	//
	conds.clear();
	if (!def)
	{
		return true;
	}

	def->reset();
	bool ret;
	while (def->hasMore())
	{
		OmnXmlItemPtr item = def->next();
		OmnString tagId = item->getTag();

		if (tagId == "ArithCond" || tagId == "NameValueCond")
		{
			ret = parseArithNameValue(item, conds);
		}
		else if (tagId == "ReturnCheck1")
		{
			ret = AosCondReturnValue::parseCond(item, conds);
		}
		else
		{
			OmnAlarm << "Unrecognized condition: " << item->getTag() << enderr;
			return false;
		}

		if (!ret)
		{
			OmnAlarm << "Failed to parse condition: " << def->toString()
				<< enderr;
			return false;
		}
	}

	return true;
}


bool
AosTortCond::parseArithNameValue(const OmnXmlItemPtr &item,
					OmnDynArray<AosTortCondPtr> &conds)
{
	OmnString contents = item->getStr();
	OmnStrParser parser(contents);
	OmnString parmName1 = parser.nextWord("", " ");
	OmnString opr = parser.nextWord("", " ");
	OmnString parmName2 = parser.nextWord("", " ");
	if (parmName1 == "" || parmName2 == "" || opr == "")
	{
		OmnAlarm << "Incorrect conditions: " << item->toString() << enderr;
		return false;
	}

	AosTortCondOpr theOpr = AosTorCondOpr_toEnum(opr);
	if (theOpr == eAosTortCondOpr_Invalid)
	{
		OmnAlarm << "Invalid operator: " << item->toString() << enderr;
		return false;
	}

	AosTortCondPtr cond;
	if (item->getTag() == "ArithCond")
	{
		cond = OmnNew AosTortCondArith(parmName1, parmName2, theOpr);
		conds.append(cond);
	}
	else if (item->getTag() == "NameValueCond")
	{
		cond = OmnNew AosTortCondNameValue(parmName1, parmName2, theOpr);
		conds.append(cond);
	}
	else
	{
		OmnAlarm << "Unrecognized condition: " << item->getTag() << enderr;
		return false;
	}

	return true;
}


