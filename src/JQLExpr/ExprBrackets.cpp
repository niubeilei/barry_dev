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
// 2014/10/25 Created by Andy
////////////////////////////////////////////////////////////////////////////
#include "JQLExpr/ExprBrackets.h"
#include "JQLStatement/JqlStatement.h"
#include "API/AosApi.h"
#include "Alarm/Alarm.h"


AosExprBrackets::AosExprBrackets(const AosExprObjPtr &expr)
:
mExprPtr(expr),
mExpr(mExprPtr.getPtr())
{
	aos_assert(expr);
}


AosExprBrackets::AosExprBrackets(const vector<AosExprObjPtr> &expr_list)
:
mExprList(expr_list)
{
}


AosExprBrackets::~AosExprBrackets()
{
}


AosDataType::E
AosExprBrackets::getDataType(
		AosRundata *rdata,
		AosDataRecordObj *record)
{
	aos_assert_r(mExpr, AosDataType::eInvalid);
	return mExpr->getDataType(rdata, record);
}


bool
AosExprBrackets::getValue(
		AosRundata *rdata,
		AosDataRecordObj *record,
		AosValueRslt &value) 
{
	aos_assert_r(mExpr, false);
	return mExpr->getValue(rdata, record, value);
}


bool
AosExprBrackets::mergeTerms(
		const AosRundataPtr &rdata, 
		bool &merged, 
		AosExprObjPtr &merged_term)
{
	return mExpr->mergeTerms(rdata, merged, merged_term);
}


AosExprObjPtr                                                                     
AosExprBrackets::cloneExpr() const                                                 
{                                                                               
	try                                                                         
	{                                                                           
		AosExprObjPtr expr= mExpr->cloneExpr();                                                
		aos_assert_r(expr, 0);                                                  
		AosExprBracketsPtr vv = OmnNew AosExprBrackets(expr);               
		return vv;                                                            
	}                                                                           

	catch (...)                                                                 
	{                                                                           
		OmnAlarm << "failed create expr" << enderr;                             
		return 0;                                                               
	}                                                                           

	OmnShouldNeverComeHere;                                                     
	return 0;                                                                   
}                                                                               



OmnString 
AosExprBrackets::dump() const 
{
	OmnString str;
	if (mExpr)
	{
		str << '(' << mExpr->dump() << ')' << "";
	}
	else
	{
		str << '(';
		for (u32 i=0; i<mExprList.size(); i++)
		{
			if (i>0) str << ',';
			str << mExprList[i]->dump();
		}
		str << ')';
	}
	return str;
}


OmnString 
AosExprBrackets::dumpByNoEscape() const 
{
	OmnString str;
	str << '(' << mExpr->dumpByNoEscape() << ')' << "";
	return str;
}


OmnString 
AosExprBrackets::dumpByStat() const 
{
	OmnString str;
	str << '(' << mExpr->dumpByStat() << ')' << "";
	return str;
}


int
AosExprBrackets::getNumFieldTerms() const
{
	if (mExpr)
	{
		return mExpr->getNumFieldTerms();
	}

	int sum = 0;
	for (u32 i=0; i<mExprList.size(); i++)
	{
		sum += mExprList[i]->getNumFieldTerms();
	}

	return sum;
}


bool
AosExprBrackets::fieldExprWithAggr() const 
{
	return mExpr->fieldExprWithAggr();
}


AosJimoPtr
AosExprBrackets::cloneJimo() const
{
	return OmnNew AosExprBrackets(*this);
}

