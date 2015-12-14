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
// 	<cond type="xxx" opr="xxx">
// 		<lhs .../>
// 		<rhs .../>
// 	</cond>
//
// Modification History:
// 12/24/2010: Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "Conds/CondSArith.h"

#include "Debug/Except.h"
#include "Random/CommonValues.h"
#include "Random/RandomUtil.h"
#include "ValueSel/ValueSel.h"


AosCondSArith::AosCondSArith(const bool reg)
:
AosCondition(AOSCONDTYPE_ARITH, AosCondType::eArith, reg)
{
}


AosCondSArith::~AosCondSArith()
{
}


bool 
AosCondSArith::evalCond(const AosXmlTagPtr &def, const AosRundataPtr &rdata)
{
	// 	<cond zky_type="xxx" opr="xxx">
	// 		<AOSTAG_LHS zky_data_type="int/string".../>
	// 		<AOSTAG_RHS .../>
	// 	</cond>
	//
	if (!def)
	{
		AosSetError(rdata, "cond_failed_001");
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	AosXmlTagPtr tt = def->getFirstChild(AOSTAG_LHS);
	if (!tt)
	{
		AosSetError(rdata, "cond_failed_002:") << def->toString();
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	AosValueRslt lhs, rhs;
	if (!AosValueSel::getValueStatic(lhs, tt, rdata))
	{
		// Failed retrieving the value.
		AosSetError(rdata, "cond_failed_003:") << def->toString();
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	tt = def->getFirstChild(AOSTAG_RHS);
	if (!tt)
	{
		AosSetError(rdata, "cond_failed_004:") << def->toString();
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	if (!AosValueSel::getValueStatic(rhs, tt, rdata))
	{
		// Failed retrieving the value.
		AosSetError(rdata, "cond_failed_005:") << def->toString();
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	OmnString oprstr = def->getAttrStr(AOSTAG_OPERATOR);
	if (oprstr == "")
	{
		AosXmlTagPtr opr = def->getFirstChild(AOSTAG_OPR);
		if (!opr)
		{
			AosSetError(rdata, "cond_failed_006:") << def->toString();
			OmnAlarm << rdata->getErrmsg() << enderr;
			return false;
		}
		oprstr = opr->getNodeText(); 
	}

	// by andy
	if (lhs.isNull() || rhs.isNull()) return false;

	AosDataType::E type = AosDataType::autoTypeConvert(lhs.getType(), rhs.getType());
	AosValueRslt vv;
	switch (toEnum(oprstr))
	{
	case eEqual: 		
		return AosValueRslt::doComparison(eAosOpr_eq, type, lhs, rhs);
	case eNotEqual:		
		return AosValueRslt::doComparison(eAosOpr_ne, type, lhs, rhs);
	case eLT:			
		return AosValueRslt::doComparison(eAosOpr_lt, type, lhs, rhs); 
	case eLE:			
		return AosValueRslt::doComparison(eAosOpr_le, type, lhs, rhs);
	case eGT:		
		return AosValueRslt::doComparison(eAosOpr_gt, type, lhs, rhs);
	case eGE:	
		return AosValueRslt::doComparison(eAosOpr_ge, type, lhs, rhs);
	default:			break;
	}

	AosSetError(rdata, "cond_failed_007") << ": " << def->toString();
	OmnAlarm << rdata->getErrmsg() << enderr;
	return false;
}


AosConditionObjPtr
AosCondSArith::clone()
{
	try
	{
		return OmnNew AosCondSArith(false);
	}
	catch (...)
	{
		OmnAlarm << "Failed to create cond" << enderr;
		return 0;
	}

	OmnShouldNeverComeHere;
	return 0;
}


OmnString 
AosCondSArith::getXmlStr(
		const OmnString &tagname,
		const int level,
		const OmnString &then_part,
		const OmnString &else_part,
		const AosRandomRulePtr &rule, 
		const AosRundataPtr &rdata)
{
	OmnString str = "<";
	OmnString tname = getTagname(tagname, rule);
	addHeader(str, tname, AOSCONDTYPE_AND, rule);
	str << " " << AOSTAG_OPERATOR << "=\"" << pickOperator() << "\">";

	/*
	str << AosValueRandUtil::pickLhsSelector(AOSTAG_LHS, level-1, 
			AosCommonValues::pickAttrName(), rule, rdata)
		<< AosValueRandUtil::pickRhsSelector(AOSTAG_RHS, level-1, 
			AosCommonValues::pickAttrValue(), rule, rdata);
	*/
	addThenElse(str, then_part, else_part, tname);
	return str;
}


OmnString
AosCondSArith::generateCommand(const AosXmlTagPtr &sdoc, const AosRundataPtr &rdata)
{
	OmnNotImplementedYet;
	return "";
}


