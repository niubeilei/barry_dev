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
// 2014/04/08 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "Funcs/Jimos/GetDocid.h"

#include "API/AosApiP.h"
#include "Rundata/Rundata.h"
#include "SEInterfaces/ExprObj.h"
#include "XmlUtil/XmlTag.h"

#include "JQLStatement/JqlStmtSchema.h"
#include "SEInterfaces/DataFieldObj.h"
#include "API/AosApiS.h"


extern "C"
{

AosJimoPtr AosCreateJimoFunc_AosGetDocid_1(const AosRundataPtr &rdata, const int version)
{
	try
	{
		OmnScreen << "To create Jimo: " << endl;
		AosJimoPtr jimo = OmnNew AosGetDocid(version);
		aos_assert_r(jimo, 0);
		return jimo;
	}

	catch (...)
	{
		AosSetErrorU(rdata, "Failed creating jimo") << enderr;
		return 0;
	}

	OmnShouldNeverComeHere;
	return 0;
}
}


AosGetDocid::AosGetDocid(const int version)
:
AosGenFunc("GetDocid", version),
mExpr(NULL),
mExprRaw(0)
{
}

AosGetDocid::AosGetDocid()
:
AosGenFunc("GetDocid", 1),
mExpr(NULL),
mExprRaw(0)
{
}



AosGetDocid::AosGetDocid(const AosGetDocid &rhs)
:
AosGenFunc(rhs)
{
	mExpr =  NULL;
	mExprRaw = NULL;
}


AosGetDocid::~AosGetDocid()
{
}


bool
AosGetDocid::config(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &worker_doc,
		const AosXmlTagPtr &jimo_doc) 
{
	return true;
}


AosDataType::E 
AosGetDocid::getDataType(
		AosRundata *rdata, 
		AosDataRecordObj *record)
{
	return AosDataType::eInt64;
}


bool
AosGetDocid::getValue(
		AosRundata *rdata, 
		AosValueRslt &value, 
		AosDataRecordObj *record)
{
	if (!record) return false;
	if (mExprRaw)
	{
		return mExprRaw->getValue(rdata, record, value);
	}
	else
	{
		OmnString docid = "docid", err;
		bool rslt = AosGenFunc::getValue(rdata, 0, record);
		aos_assert_r(rslt, false);

		OmnString schemaName = mValue.getStr();
		aos_assert_r(schemaName != "", false);

		int idx = record->getFieldIdx(docid, rdata);
		if (idx < 0)
		{
			OmnString objid = AosJqlStatement::getObjid(rdata, JQLTypes::eSchemaDoc, schemaName);
			aos_assert_r(objid != "", false);

			OmnString str = "<datafield zky_name=\"docid\" type=\"docid\"";
			str << " zky_record_objid=\"" << objid << "\" />";

			AosXmlTagPtr xml = AosStr2Xml(rdata, str AosMemoryCheckerArgs);
			aos_assert_r(xml, false);

			AosDataFieldObjPtr field = AosDataFieldObj::createDataFieldStatic(xml, record, rdata);
			aos_assert_r(field, false);

			rslt = record->appendField(rdata, field);
			aos_assert_r(rslt, false);
		}

		docid << ";";
		mExpr = AosParseExpr(docid, err, rdata);      
		if (!mExpr)
		{
			AosSetErrorUser(rdata, "syntax_error")
				<< "CondExpr AosParseJQL ERROR:: " 
				<< "" << enderr;
			return false;
		}
		mExprRaw = mExpr.getPtr();

		rslt = mExprRaw->getValue(rdata, record, value);  
		aos_assert_r(rslt, false);
		return true;
	}
}


bool 
AosGetDocid::syntaxCheck(
		const AosRundataPtr &rdata, 
		OmnString &errmsg)
{
	//check if the number of arguments == 1
	if (mSize != 1)
	{
		errmsg << "GetDocid need 1 parameter";
		return false;
	}

	return true;
}


AosJimoPtr
AosGetDocid::cloneJimo()  const
{
	try
	{
		return OmnNew AosGetDocid(*this);
	}

	catch (...)
	{
		OmnAlarm << "Failed clone Jimo" << enderr;
	}

	OmnShouldNeverComeHere;
	return 0;
}
