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
// Modification History:
// 2014/11/20 Created by Barry Niu
////////////////////////////////////////////////////////////////////////////
#include "JQLExpr/ExprIsNotNull.h"
#include "Debug/Except.h"
#include "Util/OmnNew.h"
#include "API/AosApi.h"
#include "JQLStatement/JqlStmtTable.h"
#include "JQLStatement/JqlStatement.h"
#include "Util/DataTypes.h"


AosExprIsNotNull::AosExprIsNotNull(
		const AosExprObjPtr	&field)
:
mFieldPtr(field),
mField(0)
{
	mField = mFieldPtr.getPtr();
	if (!mField)
	{
		return;
	}
}


AosExprIsNotNull::AosExprIsNotNull(const AosExprIsNotNull &rhs)
:
mField(0)
{
	if (rhs.mFieldPtr)
	{
		mFieldPtr = rhs.mField->cloneExpr();
		mField = mFieldPtr.getPtr();
	}
}


AosExprIsNotNull::~AosExprIsNotNull()
{
}


bool
AosExprIsNotNull::getValue(
		AosRundata *rdata,
		AosDataRecordObj *record,
		AosValueRslt &value)
{
	if (!mField)
	{
		AosSetErrorUser(rdata, __func__)
			<< "Field Is NULL" << enderr;
		OmnAlarm << __func__ << enderr;
		return false;
	}

	AosValueRslt vv;
	bool rslt= mField->getValue(rdata, record, vv);
	aos_assert_rr(rslt, rdata, false);
	value.setBool(!vv.isNull());
	return true;
}

bool
AosExprIsNotNull::getValue(
		AosRundata *rdata,
		const AosValueRslt &key,
		AosValueRslt &value)
{
	value.setBool(!key.isNull());
	return true;
}


AosExprObjPtr
AosExprIsNotNull::cloneExpr() const                            
{
	return OmnNew AosExprIsNotNull(*this);
}


int
AosExprIsNotNull::getNumFieldTerms() const
{
	return 1;
}

	
bool		
AosExprIsNotNull::getFields(
		const AosRundataPtr &rdata,
		vector<AosJqlSelectFieldPtr> &fields)
{
	if (mField) mField->getFields(rdata, fields);
	return true;
}

	
bool
AosExprIsNotNull::createConds(
		const AosRundataPtr &rdata, 
		vector<AosJqlQueryWhereCondPtr> &conds,
		const OmnString &tableName)
{
	//get FieldType
	AosJqlQueryWhereCond *cond = new AosJqlQueryWhereCond;
	cond->mLHS = mField->getValue(rdata.getPtrNoLock());
	cond->mRHS = "";
	
	AosDataType::E fieldType = getDataType(rdata,tableName);

	switch(fieldType)
	{
	case AosDataType::eString:
		cond->mRHS << '\b';
		break;

	case AosDataType::eU64:
		cond->mRHS << (((u64)0xffffffffffffffff) - 8);
		break;

	case AosDataType::eInt64:
		cond->mRHS << (((i64)0x7fffffffffffffff) - 8);
		break;

	case AosDataType::eDateTime:
		cond->mRHS << (((i64)0x7fffffffffffffff) - 8);
		break;

	case AosDataType::eDouble:
		cond->mRHS << (((d64)0x7fffffffffffffff) - 8);
		break;
	
	default:
		return false;
	}

	//cond->mRHS << '\b';
	cond->mOpr = "ne";
	cond->mExpr = this;
	conds.push_back(cond);
	return true;
}


OmnString 
AosExprIsNotNull::dump() const
{
	OmnString str;
	str <<  mField->dump() << " IS NOT NULL " ;
	return str;
}

OmnString 
AosExprIsNotNull::dumpByNoEscape() const
{
	OmnString str;
	str <<  mField->dumpByNoEscape() << " IS NOT NULL " ;
	return str;
}

OmnString 
AosExprIsNotNull::dumpByStat() const
{
	return dump();
}

AosDataType::E
AosExprIsNotNull::getDataType(
		const AosRundataPtr &rdata,
		const OmnString &tableName)
{
	OmnString fieldName = mField->dumpByNoEscape();
	AosXmlTagPtr table_doc = AosJqlStmtTable::getTable(tableName, rdata); 
	//aos_assert_r(table_doc, NULL);
	OmnString objid = table_doc->getAttrStr("zky_use_schema","");
	AosXmlTagPtr schemaDoc = AosJqlStatement::getDocByObjid(rdata, objid);
	//aos_assert_r(schemaDoc, NULL);
	AosXmlTagPtr dataRecordDoc = schemaDoc->getFirstChild("datarecord");
	//aos_assert_r(schemaDoc, NULL);
	AosXmlTagPtr fieldsDoc = dataRecordDoc->getFirstChild("datafields");
	//aos_assert_r(schemaDoc, NULL);
	
	AosDataType::E fieldType;
	AosXmlTagPtr fieldDoc = fieldsDoc->getFirstChild();
	OmnString type = "";
	while(fieldDoc)
	{
		if(fieldName == fieldDoc->getAttrStr("zky_name",""))
		{
			type = fieldDoc->getAttrStr("type","");
			if(type == "expr")
				type = fieldDoc->getAttrStr("data_type","");
			break;
		}
		fieldDoc = fieldsDoc->getNextChild();
	}
	fieldType = AosDataType::toEnum(type);
	return fieldType;
}
