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
// 2013/10/08 Created by Barry Niu
////////////////////////////////////////////////////////////////////////////
#include "JQLExpr/ExprContain.h"

#include "WordParser/WordParser.h"
#include "WordParser/WordNorm.h"     

#include "API/AosApi.h"
#include "Util/OmnNew.h"

AosExprContain::AosExprContain()
:
mField(0)
{
}



AosExprContain::AosExprContain(
		AosExprObjPtr field, 
		char *value)
:
mFieldPtr(field),
mField(mFieldPtr.getPtr()),
mValue(value)
{
	if (!field)
	{
		OmnAlarm << "ExprContain Field is null." << enderr;
	}
	if (!value)
	{
		OmnAlarm << "ExprContain Values is null." << enderr;
	}
}


AosExprContain::~AosExprContain()
{
}


bool
AosExprContain::getValue(
		AosRundata *rdata,
		AosDataRecordObj *record,
		AosValueRslt &value)
{
	aos_assert_r(NULL, false);
	aos_assert_r(record, false);
	bool rslt = record->getFieldValue(mValue, value, true, rdata);
	aos_assert_r(rslt, false);
	AosDataType::E type = value.getType();
	if (type == AosDataType::eString 
			|| type == AosDataType::eCharStr 
			|| type == AosDataType::eChar)
	{
		OmnString data = value.getStr();
		data.removeLeadingWhiteSpace();
		value.setBool(check(data));
		return true;
	}
	OmnString errmgr = "Record is not find this field : field name is ";
	errmgr << mValue;

	AosSetEntityError(rdata, errmgr, 
			"Deny Reason", errmgr) << enderr;
	return false;
}


bool
AosExprContain::getValue(
		AosRundata *rdata,
		const AosValueRslt &key,
		AosValueRslt &value) 
{
	aos_assert_r(NULL, false);
	OmnString data = key.getStr();
	aos_assert_r(data != "", false);
	value.setBool(check(data));
	return false;
}


bool
AosExprContain::check(OmnString data) const
{
	bool rslt;
	OmnString value;
	AosWordParserPtr wordpaeser = OmnNew AosWordParser();
	aos_assert_r(wordpaeser, false);
	wordpaeser->setSrc(mValue.data(), 0, mValue.length());
	char word_str[AosXmlTag::eMaxWordLen+1];
	int str_len;

	while (1)
	{
		rslt = wordpaeser->nextWord(word_str, str_len);
		if (!rslt) break;
		value = OmnString(word_str, str_len);
		if (data == value) return true;
	}
	return false;
}


bool                               
AosExprContain::createConds(     
		const AosRundataPtr &rdata,
		vector<AosJqlQueryWhereCondPtr> &conds,
		const OmnString &tableName)
{                                  

	AosJqlQueryWhereCondPtr	cond = OmnNew AosJqlQueryWhereCond;
	OmnString str = ""; 

	cond->mLHS = mField->getValue(rdata.getPtrNoLock());
	cond->mRHS = mValue;
	cond->mOpr = "contain";
	conds.push_back(cond);
	return true;
}                                  


bool
AosExprContain::getFields(
	const AosRundataPtr &rdata,
	vector<AosJqlSelectFieldPtr> &fields)
{
	return mField->getFields(rdata, fields); 
}


AosExprObjPtr
AosExprContain::cloneExpr() const                            
{
	try
	{
		if (mField && mValue != "")
		{
			AosExprContainPtr expr = OmnNew AosExprContain();
			expr->mFieldPtr = mFieldPtr->cloneExpr();
			expr->mField = expr->mFieldPtr.getPtr();
			expr->mValue = mValue;
			return expr; 
		}
		OmnAlarm << "AosExprContain clone is failed" << enderr;
		return 0;
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
AosExprContain::dump() const
{
	OmnString str;
	str << mField->dump();
	str << " contain \"";
	str << mValue << "\"";
	return str;
}


OmnString 
AosExprContain::dumpByNoEscape() const
{
	OmnString str;
	str << mField->dumpByNoEscape();
	str << " contain \"";
	str << mValue << "\"";
	return str;
}

OmnString 
AosExprContain::dumpByStat() const
{
	OmnString str;
	str << mField->dumpByStat();
	str << " contain \"";
	str << mValue << "\"";
	return str;
}

