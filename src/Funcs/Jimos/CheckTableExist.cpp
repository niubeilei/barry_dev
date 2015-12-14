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
// 2015/01/30 Created by Ken Lee
////////////////////////////////////////////////////////////////////////////
#include "Funcs/Jimos/CheckTableExist.h"

#include "API/AosApi.h"
#include "JQLStatement/JqlStmtTable.h"
#include "Rundata/Rundata.h"
#include "SEInterfaces/ExprObj.h"
#include "XmlUtil/XmlTag.h"


extern "C"
{

AosJimoPtr AosCreateJimoFunc_AosCheckTableExist_1(const AosRundataPtr &rdata, const int version)
{
	try
	{
		OmnScreen << "To create Jimo: " << endl;
		AosJimoPtr jimo = OmnNew AosCheckTableExist(version);
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


AosCheckTableExist::AosCheckTableExist()
:
AosGenFunc("check_database_exist", 1)
{
}


AosCheckTableExist::AosCheckTableExist(const int version)
:
AosGenFunc("check_database_exist", version)
{
}


AosCheckTableExist::AosCheckTableExist(const AosCheckTableExist &rhs)
:
AosGenFunc(rhs)
{
}


AosCheckTableExist::~AosCheckTableExist()
{
}


bool
AosCheckTableExist::config(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &worker_doc,
		const AosXmlTagPtr &jimo_doc) 
{
	return true;
}


AosDataType::E 
AosCheckTableExist::getDataType(
		AosRundata *rdata, 
		AosDataRecordObj *record)
{
	return AosDataType::eBool;
}


bool
AosCheckTableExist::getValue(
		AosRundata *rdata, 
		AosValueRslt &value, 
		AosDataRecordObj *record)
{
	OmnString db_name, table_name; 
	AosGenFunc::getValue(rdata, 0, record);
	db_name = mValue.getStr();
	AosGenFunc::getValue(rdata, 1, record);
	table_name = mValue.getStr();

	bool rslt = AosJqlStmtTable::checkTable(rdata, db_name, table_name);
	value.setBool(rslt);
	return true;
}


bool 
AosCheckTableExist::syntaxCheck(
		const AosRundataPtr &rdata, 
		OmnString &errmsg)
{
	if (mSize != 2)
	{
		errmsg << "CheckTableExist needs only 1 parameters.";
		return false;
	}

	return true;
}


AosJimoPtr
AosCheckTableExist::cloneJimo() const
{
	try
	{
		return OmnNew AosCheckTableExist(*this);
	}

	catch (...)
	{
		OmnAlarm << "Failed creating dataset" << enderr;
	}

	OmnShouldNeverComeHere;
	return 0;
}

