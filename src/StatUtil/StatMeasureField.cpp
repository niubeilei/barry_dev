////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
//
// Modification History:
// 2014/07/25 Created by Ketty 
////////////////////////////////////////////////////////////////////////////

#include "StatUtil/StatMeasureField.h"
#include "XmlUtil/XmlTag.h"
#include "SEUtil/JqlUtil.h"
AosStatMeasureField::AosStatMeasureField()
:
AosStatField(eMeasureField)
{
}

AosStatMeasureField::AosStatMeasureField(
		const OmnString &fname,
		const OmnString &raw_fname,
		const OmnString &aggr_func_str)
:
AosStatField(eMeasureField),
mAggrFuncStr(aggr_func_str),
mRawFname(raw_fname)
{
	setFieldName(fname);
}


bool
AosStatMeasureField::config(const AosXmlTagPtr &conf)
{
	bool rslt = AosStatField::config(conf);	
	aos_assert_r(rslt, false);
	
	mAggrFuncStr = conf->getAttrStr("agr_type", "");
	aos_assert_r(mAggrFuncStr != "", false);
	
	mRawFname = conf->getAttrStr("raw_field_name", "");
	aos_assert_r(mRawFname != "", false);
	
	OmnString isMeasureCond = conf->getAttrStr("is_aggr_cond","");
	if(isMeasureCond == "true")
	{
		AosRundataPtr rdata = OmnApp::getRundata();
		OmnString errmsg,expr_str;
		expr_str = conf->getNodeText();
		expr_str <<";";
		mCond = AosParseExpr(expr_str,errmsg,rdata.getPtr());
		aos_assert_r(mCond,false);
		
		if(mCond)
		{
		mCondMeasureName = "";
		mCondMeasureName << mAggrFuncStr << "(" << mRawFname << "," << mCond->dumpByNoEscape() << ")";
		AosJqlUtil::escape(mCondMeasureName);
		}
	}

	return true;
}


OmnString
AosStatMeasureField::toXmlString()
{
	OmnString str = "<field ";
	bool rslt = AosStatField::toXmlString(str);
	aos_assert_r(rslt, "");

	str << "agr_type=\"" << mAggrFuncStr << "\" "
		<< "raw_field_name=\"" << mRawFname << "\" ";
	if(mCond)
	{
		str << "is_aggr_cond=\"true\"" 
			<<"><![CDATA["<<mCond->dump()<<"]]>";
	}
	else
		str << ">";
	str<<"</field>";

	return str;
}


