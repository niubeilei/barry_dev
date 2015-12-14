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
#include "Funcs/Jimos/GetMetaData.h"

#include "API/AosApiP.h"
#include "Rundata/Rundata.h"
#include "SEInterfaces/ExprObj.h"
#include "XmlUtil/XmlTag.h"

#include "JQLStatement/JqlStmtSchema.h"
#include "SEInterfaces/DataFieldObj.h"
#include "API/AosApiS.h"


extern "C"
{

AosJimoPtr AosCreateJimoFunc_AosGetMetaData_1(const AosRundataPtr &rdata, const int version)
{
	try
	{
		OmnScreen << "To create Jimo: " << endl;
		AosJimoPtr jimo = OmnNew AosGetMetaData(version);
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


AosGetMetaData::AosGetMetaData(const int version)
:
AosGenFunc("GetMetaData", version),
mExprRaw(0)
{
}

AosGetMetaData::AosGetMetaData()
:
AosGenFunc("GetMetaData", 1),
mExprRaw(0)
{
}



AosGetMetaData::AosGetMetaData(const AosGetMetaData &rhs)
:
AosGenFunc(rhs)
{
	mExpr = NULL;
	mExprRaw = NULL;
}


AosGetMetaData::~AosGetMetaData()
{
}


bool
AosGetMetaData::config(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &worker_doc,
		const AosXmlTagPtr &jimo_doc) 
{
	return true;
}


AosDataType::E 
AosGetMetaData::getDataType(
		AosRundata *rdata, 
		AosDataRecordObj *record)
{
	return AosDataType::eString;
}


bool
AosGetMetaData::getValue(
		AosRundata *rdata, 
		AosValueRslt &value, 
		AosDataRecordObj *record)
{
	if (!record) return false;
	bool rslt = false;
	if (mExprRaw)
	{
		return mExprRaw->getValue(rdata, record, value);  
	}
	else
	{
		OmnString fieldName, err;
		rslt = AosGenFunc::getValue(rdata, 0, record);
		aos_assert_r(rslt, false);

		OmnString name = mValue.getStr();
		aos_assert_r(name == "filename" || name == "offset" || name == "length" || name == "docid", false);

		OmnString metaName = "zky_meta_";
		metaName << name;
		int idx = record->getFieldIdx(metaName, rdata);
		if (idx < 0)
		{
			OmnString str = "<datafield zky_name=\"";
			str << metaName << "\" type=\"metadata\" zky_metadata_attrname=\""
				<< name << "\" />";

			AosXmlTagPtr xml = AosStr2Xml(rdata, str AosMemoryCheckerArgs);
			aos_assert_r(xml, false);

			AosDataFieldObjPtr field = AosDataFieldObj::createDataFieldStatic(xml, record, rdata);
			aos_assert_r(field, false);

			rslt = record->appendField(rdata, field);
			aos_assert_r(rslt, false);
		}

		metaName << ";";
		mExpr = AosParseExpr(metaName, err, rdata);      
		if (!mExpr)
		{
			AosSetErrorUser(rdata, "syntax_error")
				<< "CondExpr AosParseJQL ERROR:: " 
				<< "" << enderr;
			return false;
		}
		mExprRaw = mExpr.getPtr();
		return mExprRaw->getValue(rdata, record, value);  
	}
	return true;

}


bool 
AosGetMetaData::syntaxCheck(
		const AosRundataPtr &rdata, 
		OmnString &errmsg)
{
	//check if the number of arguments == 1
	if (mSize != 1)
	{
		errmsg << "GetMetaData need 1 parameter";
		return false;
	}

	return true;
}


AosJimoPtr
AosGetMetaData::cloneJimo()  const
{
	try
	{
		return OmnNew AosGetMetaData(*this);
	}

	catch (...)
	{
		OmnAlarm << "Failed clone Jimo" << enderr;
	}

	OmnShouldNeverComeHere;
	return 0;
}
