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
// 07/28/2014 Created by Young
////////////////////////////////////////////////////////////////////////////
#include "JQLExpr/ExprLike.h"

#include "API/AosApi.h"
#include "Alarm/Alarm.h"
#include "JQLStatement/JqlStmtTable.h"
#include "SEUtil/IILName.h"

#include <boost/regex.hpp>

AosExprLike::AosExprLike(
		AosExprObjPtr lhs, 
		AosExprObjPtr rhs, 
		const bool not_like)
:
AosExprBinary(lhs, rhs),
mNotLike(not_like)
{
	AosRundataPtr rdata = OmnApp::getRundata();
	rdata->setSiteid(100);
//	aos_assert(rhs->getType() == AosExprType::eString);
	if (rhs->getType() == AosExprType::eString)
	{
		OmnString v_str = rhs->getValue(rdata.getPtrNoLock());
		aos_assert(v_str != "");
		
		v_str = convertPatternFromSqlToBoost(v_str);
		aos_assert(v_str != "");
	
		string str = v_str.data();
		mReg = boost::regex(str);
	}
}


AosExprLike::~AosExprLike()
{
}


bool
AosExprLike::getValue(
		AosRundata *rdata,
		AosDataRecordObj *record,
		AosValueRslt &value)
{
	aos_assert_r(mLHS, false);
	aos_assert_r(mRHS, false);

	AosValueRslt v1, v2;
	if (!mLHS->getValue(rdata, record, v1)) return false;
	string str1 = v1.getStr();

	// Add by Young, 2015/03/04
	if (!mRHS->isConstant())
	{
	    if (!mRHS->getValue(rdata, record, v2)) return false;
	    string str2 = convertPatternFromSqlToBoost(v2.getStr());
		mReg = boost::regex(str2);
	}

	if (boost::regex_match(str1, mReg))
	{
		value.setBool(mNotLike ? false : true);
	}
	else
	{
		value.setBool(mNotLike ? true : false);
	}
	
	return true;
}


bool
AosExprLike::getValue(
	AosRundata *rdata,
	vector<AosDataRecordObj *> &records,
	AosValueRslt &value)
{
	aos_assert_r(mLHS, false);
	aos_assert_r(mRHS, false);

	AosValueRslt v1, v2;
	if (!mLHS->getValue(rdata, records, v1)) return false;
	string str1 = v1.getStr();

	// Add by Young, 2015/03/04
	if (!mRHS->isConstant())
	{
	    if (!mRHS->getValue(rdata, records, v2)) return false;
	    string str2 = convertPatternFromSqlToBoost(v2.getStr());
		mReg = boost::regex(str2);
	}

	if (boost::regex_match(str1, mReg))
	{
		value.setBool(mNotLike ? false : true);
	}
	else
	{
		value.setBool(mNotLike ? true : false);
	}
	
	return true;


}

bool
AosExprLike::getValue(
		AosRundata *rdata,
		const AosValueRslt &key,
		AosValueRslt &value) 
{
	aos_assert_r(mLHS, false);

	OmnString vv1 = key.getStr();
	string str1 = vv1.data();

	if (boost::regex_match(str1, mReg))
	{
		value.setBool(mNotLike ? false : true);
	}
	else
	{
		value.setBool(mNotLike ? true : false);
	}
	
	return true;
}


bool 
AosExprLike::moveTerms(
		const AosRundataPtr &rdata, 
		bool &moved, 
		AosExprObjPtr &term)
{
	moved = false;
	return true;
}


bool
AosExprLike::createConds(
		const AosRundataPtr &rdata,
		vector<AosJqlQueryWhereCondPtr> &conds,
		const OmnString &tableName)
{                                  
	aos_assert_r(mLHS, false);
	aos_assert_r(mRHS, false);
	AosJqlQueryWhereCond *cond = new AosJqlQueryWhereCond;
	cond->mLHS = mLHS->getValue(rdata.getPtrNoLock());
	cond->mRHS = mRHS->getValue(rdata.getPtrNoLock());
	cond->mOpr = "like";
	if (mNotLike) 
	{
		cond->mOpr = "not like";
		//cond->mExpr = cloneExpr();
	}

	cond->mExpr = cloneExpr();
	conds.push_back(cond);
	return true;
}                                  


AosExprObjPtr                                                           
AosExprLike::cloneExpr() const                                       
{                                                                     
	try                                                               
	{                                                                 
		AosExprObjPtr lhs, rhs;                                        
		lhs = mLHS->cloneExpr();                                      
		aos_assert_r(lhs, 0);                                         
		rhs = mRHS->cloneExpr();                                      
		aos_assert_r(rhs, 0);                                         
		AosExprLikePtr expr = OmnNew AosExprLike(lhs, rhs, mNotLike);     
		return expr;                                                  
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
AosExprLike::dump() const
{
	OmnString str = "";
	str << mLHS->dump();

	if (mNotLike)
		str << " NOT ";

	str << " LIKE " << mRHS->dump(); 
	return str;
}

OmnString
AosExprLike::dumpByNoEscape() const
{
	OmnString str = "";
	str << mLHS->dumpByNoEscape();

	if (mNotLike)
		str << " NOT ";

	str << " LIKE " << mRHS->dumpByNoEscape(); 
	return str;
}

OmnString
AosExprLike::dumpByStat() const
{
	return dump();
}


OmnString 
AosExprLike::convertPatternFromSqlToBoost(const OmnString src) const
{
	OmnString str = src;
    OmnString newstr = "";
    str.replace("\\\\", "\\", true);
    bool flag = false;
	bool squareBracketsStart = false;
    for (int i=0; i<str.length(); i++)
    {
        if (str[i] == '\\')
        {
            flag = (!flag);
            newstr << str[i];
        }
        else
        {
            if (flag)
            {
                newstr << str[i];
                flag = false;
            }
            else
            {
                if (str[i] == '.')  newstr << "\\.";
                else if (str[i] == '+') newstr << "\\+";
                else if (str[i] == '*') newstr << "\\*";
                else if (str[i] == '%') newstr << ".*";
                else if (str[i] == '_') newstr << ".";
				else if (str[i] == '^' && !squareBracketsStart) newstr << "\\^";
				else if (str[i] == '$') newstr << "\\$";
                else newstr << str[i];
            }
        }

		if (str[i] == '[')
		{
			squareBracketsStart = true;
		}
		else
		{
			squareBracketsStart = false;
		}

    }

    return newstr;
}

