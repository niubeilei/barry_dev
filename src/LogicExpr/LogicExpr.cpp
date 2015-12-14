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
//  A Logic Expression is in the form of:
//  	$(Variable1) Operator $(Variable2)
//  It is the leave node of the inheritance hiearchy.  
//
// Modification History:
// 07/16/2007: Created by Chen Ding
//
////////////////////////////////////////////////////////////////////////////
#include "LogicExpr/LogicExpr.h"

#include "alarm/Alarm.h"
#include "Alarm/Alarm.h"
#include "LogicExpr/LogicObj.h"
#include "XmlParser/XmlItem.h"

AosLogicExpr::AosLogicExpr()
{
}


AosLogicExpr::~AosLogicExpr()
{
}


bool 	
AosLogicExpr::config(const OmnXmlItemPtr &def)
{
	// 
	// 	<Expr>
	// 		<DataType>
	// 		the left hand operand
	// 		<Operator>
	// 		the right hand operand
	// 	</Expr>
	//
	aos_assert_r(def, false);

	if (def->getTag() != "Expr")
	{
		OmnAlarm << "Expecting a Logic Expression ('Expr' tag)"
			<< " but got: " << def->toString() << enderr;
		return false;
	}

	def->reset();
	OmnXmlItemPtr type = def->next();
	OmnXmlItemPtr left = def->next();
	OmnXmlItemPtr opr  = def->next();
	OmnXmlItemPtr right = def->next();
	if (!type || !left || !opr || !right || def->hasMore() ||
		!left->isAtomicItem() || !right->isAtomicItem() || !opr->isAtomicItem())
	{
		OmnAlarm << "Invalid Logic Expression definition: " 
			<< def->toString() << enderr;
		return false;
	}

	mDataType = AosDataType_toEnum(type->getStr());
	mLeftType = AosOperandType_toEnum(left->getTag());
	mLeftOperand = left->getStr();
	mRightType = AosOperandType_toEnum(right->getTag());
	mRightOperand = right->getStr();
	mOperator = AosTorCondOpr_toEnum(opr->getStr());

	if (mLeftType == eAosOperandType_Invalid)
	{
		OmnAlarm << "Invalid operand type for the left hand side: " 
			<< def->toString() << enderr;
		return false;
	}

	if (mRightType == eAosOperandType_Invalid)
	{
		OmnAlarm << "Invalid operand type for the right hand side: " 
			<< def->toString() << enderr;
		return false;
	}

	if (mOperator == eAosTortCondOpr_Invalid)
	{
		OmnAlarm << "Invalid operator: " 
			<< def->toString() << enderr;
		return false;
	}

	return true;
}


bool
AosLogicExpr::evaluate(bool &rslt, 
					   OmnString &errmsg,
					   const AosLogicObjPtr &obj) const
{
	rslt = false;
	aos_assert_r(obj, false);

	OmnString value1, value2;
	if (!obj->getValue(mLeftType, mLeftOperand, value1, errmsg)) return false;
	if (!obj->getValue(mRightType, mRightOperand, value2, errmsg)) return false;
	if (!AosEvaluate(mDataType, value1, value2, mOperator, rslt, errmsg)) return false;

	return true;
}

