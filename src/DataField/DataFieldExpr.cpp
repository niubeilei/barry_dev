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
// Modification History:
// 2014/05/22 Created by Ken Lee
////////////////////////////////////////////////////////////////////////////
#include "DataField/DataFieldExpr.h"

#include "API/AosApi.h"


AosDataFieldExpr::AosDataFieldExpr(const bool reg)
:
AosDataField(AosDataFieldType::eExpr, AOSDATAFIELDTYPE_EXPR, reg)
{
}


AosDataFieldExpr::AosDataFieldExpr(const AosDataFieldExpr &rhs)
:
AosDataField(rhs),
mExprStr(rhs.mExprStr)
{
	if (rhs.mExpr)
	{
		mExpr = rhs.mExpr->cloneExpr();
	}
}


AosDataFieldExpr::~AosDataFieldExpr()
{
}


bool
AosDataFieldExpr::config(
		const AosXmlTagPtr &def,
		AosDataRecordObj *record,
		AosRundata *rdata)
{
	// The config format is:
	// 	<datafield datatype="xxx" ...>
	// 		<expr datatype="xxx" ...>
	// 			...
	// 		</expr>
	// 	</datafield>
	bool rslt = AosDataField::config(def, record, rdata);
	aos_assert_r(rslt, false);

	AosXmlTagPtr expr_tag = def->getFirstChild("expr");
	aos_assert_r(expr_tag, false);

	mExprStr = expr_tag->getNodeText();
	aos_assert_r(mExprStr != "", false);

	mExprStr << ";";
	// Chen Ding, 2015/01/31
	// mExpr = AosParseJQL(mExprStr, true, rdata);
	OmnString errmsg;
	mExpr = AosParseExpr(mExprStr, errmsg, rdata);
	if (!mExpr)
	{
		AosSetErrorUser(rdata, "syntax_error")
			<< "Errmsg: " << errmsg
			<< "CondExpr AosParseJQL ERROR:: " << mExprStr << enderr;
		return false;
	}

	return true;
}


bool
AosDataFieldExpr::getValueFromRecord(
		AosDataRecordObj* record,
		const char * data,
		const int len,
		int &idx,
		AosValueRslt &value, 
		const bool copy_flag,
		AosRundata* rdata)
{
	aos_assert_r(record, false);
	aos_assert_r(mExpr, false);

	bool rslt = mExpr->getValue(rdata, record, value);
	aos_assert_r(rslt, false);

	if (value.isNull()) 
	{
		value = mDftValue;
	}	
	return true;
}


AosDataFieldObjPtr 
AosDataFieldExpr::clone(AosRundata *rdata) const
{
	return OmnNew AosDataFieldExpr(*this);
}


AosDataFieldObjPtr
AosDataFieldExpr::create(                                         
		const AosXmlTagPtr &def,
		AosDataRecordObj *record,
		AosRundata *rdata) const
{
	AosDataFieldExpr * field = OmnNew AosDataFieldExpr(false);
	bool rslt = field->config(def, record, rdata);
	aos_assert_r(rslt, 0);
	return field;
}

