////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//      
// Description: just for test
//
// Modification History:
// 01/04/2013 Created by Kerong.Li
////////////////////////////////////////////////////////////////////////////

#include "JQLExpr/Expr.h"

#include "JQLStatement/JQLCommon.h"
#include "JQLStatement/Ptrs.h"
#include "JQLExpr/ExprGenFunc.h"
#include "JQLExpr/ExprBrackets.h"
#include "JQLExpr/ExprNameValue.h"
#include "JQLExpr/ExprString.h"

#include "SEInterfaces/AggregationType.h"

AosExpr::AosExpr()
:
mAliasExpr(0)
{
}

AosJimoPtr 
AosExpr::cloneJimo() const
{
	OmnShouldNeverComeHere;
	return 0;
}

OmnString 	
AosExpr::getStatFieldName()
{
	OmnShouldNeverComeHere;
	return "";
}


AosDataType::E 
AosExpr::getDataType(AosRundata *rdata, AosDataRecordObj *record)
{
	OmnShouldNeverComeHere;
	return AosDataType::eInvalid;
}


bool
AosExpr::getValue(
	AosRundata *rdata,
	vector<AosDataRecordObj *> &records,
	AosValueRslt &value)
{
	OmnShouldNeverComeHere;
	return false;
}

bool
AosExpr::mergeTerms(                     
		const AosRundataPtr &rdata,          
		bool &merged,               
		AosExprObjPtr &merged_term) 
{
	OmnShouldNeverComeHere;
	return false;
}


bool
AosExpr::getValue(
		AosRundata *rdata,
		const AosValueRslt &key,
		AosValueRslt &value)
{
	OmnShouldNeverComeHere;
	return false;
}

bool
AosExpr::isAlwaysTrueCondition() const
{
	//OmnShouldNeverComeHere;
	return false;
}

OmnString 	
AosExpr::dump() const 
{
	OmnShouldNeverComeHere;
	return "";
}

OmnString 	
AosExpr::dumpByNoEscape() const 
{
	OmnShouldNeverComeHere;
	return "";
}


OmnString 	
AosExpr::dumpByStat() const 
{
	OmnShouldNeverComeHere;
	return "";
}


bool
AosExpr::createConds(const AosRundataPtr &rdata, 
				vector<AosJqlQueryWhereCondPtr>   &conds,
				const OmnString &tableName)
{
	OmnShouldNeverComeHere;
	return false;
}


bool
AosExpr::checkIsAgrFunc(AosExprObjPtr expr)
{
	aos_assert_r(expr, false);
	OmnString func_name;
	if (expr->getType() == AosExprType::eGenFunc)
	{
		AosExprGenFuncPtr expr_func;
		expr_func = dynamic_cast<AosExprGenFunc*>(expr.getPtr());
		if (expr_func)
		{
			func_name = expr_func->getFuctName();
			AosAggregationType::E type = AosAggregationType::toEnum(func_name.toLower());
			return AosAggregationType::isValid(type);
		}                                                                                     
	}
	return false;
}


bool 
AosExpr::isAlwaysFalseCondition() const
{
	//OmnShouldNeverComeHere;
	return false;
}


bool
AosExpr::isConstant()const
{
	return false;
}

bool 
AosExpr::isFieldName() const
{
	return false;
}


bool    
AosExpr::isUnary() const
{
	return false;
}

bool 
AosExpr::isMemberOpt() const 
{
	return false;
} 

bool    
AosExpr::setExpr(AosExprObjPtr expr, int key)
{ 
	mExprs[key] = expr; 
	return true;
} 


AosExprObjPtr
AosExpr::getExpr(int key)
{
	return mExprs[key];
}


bool    
AosExpr::setAlias(AosExprObjPtr expr)
{ 
	mAliasExpr = expr;
	return true;
} 


AosExprObjPtr
AosExpr::getAlias()
{
	return mAliasExpr;
}


int     
AosExpr::getNumFieldTerms() const
{
	return 0;
}


bool
AosExpr::isHasOr() const
{
	return false;
}

bool
AosExpr::isJoin() const
{
	return false;
}

bool
AosExpr::isAlwaysFalse()
{
	return false;
}

bool	
AosExpr::isExprCond() const
{
	return false;
}

bool
AosExpr::getFields(
		const AosRundataPtr &rdata,
		vector<AosJqlSelectFieldPtr> &fields)
{
	return true;
}

bool
AosExpr::getFieldsByStat(
		const AosRundataPtr &rdata,
		vector<AosJqlSelectFieldPtr> &fields)
{
	return getFields(rdata, fields);
}

AosExprList* 
AosExpr::getParmList() const
{
	OmnNotImplementedYet;
	return 0;
}

OmnString 
AosExpr::getMember2() const
{
	OmnNotImplementedYet;
	return "";
}

OmnString 
AosExpr::getMember3() const
{
	OmnNotImplementedYet;
	return "";
}

vector<AosExprObjPtr> 
AosExpr::getExprList() 
{
	OmnNotImplementedYet;
	vector<AosExprObjPtr> tmp;
	return tmp;
}

OmnString 
AosExpr::getName() const
{
	OmnNotImplementedYet;
	return  "";
}

AosExprObjPtr 
AosExpr::getValueAsExpr() const
{
	OmnNotImplementedYet;
	return 0;
}

OmnString 
AosExpr::getXpathName() const
{
	OmnNotImplementedYet;
	return "";
}

OmnString 
AosExpr::getFuctName() const
{
	OmnNotImplementedYet;
	return "";
}

OmnString 
AosExpr::dumpByNoQuote()const
{
	OmnNotImplementedYet;
	return  "";
}


// Chen Ding, 2015/05/25
AosExprObjPtr 
AosExpr::createArrayExpr(const vector<AosExprObjPtr> &exprs)
{
	return OmnNew AosExprBrackets(exprs);
}


AosExprObjPtr 
AosExpr::createNameValueExpr(
		const OmnString &name, 
		const AosExprObjPtr &value)
{
	return OmnNew AosExprNameValue(name, value);
}


AosExprObjPtr
AosExpr::createStrExpr(const OmnString &value)
{
	return OmnNew AosExprString(value.data());
}

AosExprObjPtr 
AosExpr::getRHS() const
{
	OmnShouldNeverComeHere;
	return 0;
}

AosExprObjPtr 
AosExpr::getLHS() const
{
	OmnShouldNeverComeHere;
	return 0;
}
