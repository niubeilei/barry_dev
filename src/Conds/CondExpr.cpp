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
// This action filters data through a hash map. The map is defined by 
// a smart doc. Callers should call:
// 		evalCond(const char *data, const int len, ...);
// 'data' points to the beginning of a record; 'len' is the length of the
// record. The condition matches the substring:
// 		[data[mStartPos], data[mStartPos] + mMatchedLength]
//
// Modification History:
// 05/07/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "Conds/CondExpr.h"

#include "Alarm/Alarm.h"
#include "API/AosApi.h"
#include "Debug/Except.h"
#include "Debug/Debug.h"
#include "Rundata/Rundata.h"
#include "Util/OmnNew.h"
#include "XmlUtil/XmlTag.h"


AosCondExpr::AosCondExpr(const bool flag)
:
AosCondition(AOSCONDTYPE_EXPR, AosCondType::eExpr, flag)
{
}


AosCondExpr::AosCondExpr(
		const OmnString &expr_str,
		const AosRundataPtr &rdata)
:
AosCondition(AOSCONDTYPE_EXPR, AosCondType::eExpr, false)
{
	if(expr_str	!= "")
	{
		mExprStr = expr_str;
	}
}


AosCondExpr::~AosCondExpr()
{
}


bool 
AosCondExpr::config(const AosXmlTagPtr &def, const AosRundataPtr &rdata)
{
	aos_assert_r(def, false);

	mExprStr = def->getNodeText();
	aos_assert_r(mExprStr != "", false);

	mExprStr << ";";
	OmnString errmsg;
	mExpr = AosParseExpr(mExprStr, errmsg, rdata.getPtr());
	if (!mExpr)
	{
		AosSetErrorUser(rdata, "syntax_error")
			<< "CondExpr AosParseJQL ERROR:: " << mExprStr 
			<< "Errmsg: " << errmsg << enderr;
		return false;
	}

	return true;
}


bool
AosCondExpr::evalCond(
		const AosDataRecordObjPtr &record,
		const AosRundataPtr &rdata)
{
	aos_assert_rr(mExpr, rdata, false);
	AosValueRslt value_rslt;
	mExpr->getValue(rdata.getPtr(), record.getPtr(), value_rslt);
	return value_rslt.getBool();
}


bool
AosCondExpr::evalCond(
		const AosValueRslt &value,
		const AosRundataPtr &rdata)
{
	aos_assert_rr(mExpr, rdata, false);
	AosValueRslt value_rslt;
	mExpr->getValue(rdata.getPtr(), value, value_rslt);
	return value_rslt.getBool();
}


AosConditionObjPtr
AosCondExpr::clone() 
{
	try
	{
		AosCondExpr * cond = OmnNew AosCondExpr(false);
		cond->mExprStr = mExprStr;
		if (mExpr)
		{
			cond->mExpr = mExpr->cloneExpr();
			return cond;
		}
		cond->mExpr = 0;
		return cond;
	}

	catch (...)
	{
		OmnAlarm << "failed create cond" << enderr;
		return 0;
	}

	OmnShouldNeverComeHere;
	return 0;
}

