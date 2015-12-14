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
#include "Funcs/Jimos/CheckDatabaseExist.h"

#include "API/AosApi.h"
#include "JQLStatement/JqlStmtDatabase.h"
#include "Rundata/Rundata.h"
#include "SEInterfaces/ExprObj.h"
#include "XmlUtil/XmlTag.h"


extern "C"
{

AosJimoPtr AosCreateJimoFunc_AosCheckDatabaseExist_1(const AosRundataPtr &rdata, const int version)
{
	try
	{
		OmnScreen << "To create Jimo: " << endl;
		AosJimoPtr jimo = OmnNew AosCheckDatabaseExist(version);
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


AosCheckDatabaseExist::AosCheckDatabaseExist()
:
AosGenFunc("check_database_exist", 1)
{
}


AosCheckDatabaseExist::AosCheckDatabaseExist(const int version)
:
AosGenFunc("check_database_exist", version)
{
}


AosCheckDatabaseExist::AosCheckDatabaseExist(const AosCheckDatabaseExist &rhs)
:
AosGenFunc(rhs)
{
}


AosCheckDatabaseExist::~AosCheckDatabaseExist()
{
}


bool
AosCheckDatabaseExist::config(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &worker_doc,
		const AosXmlTagPtr &jimo_doc) 
{
	return true;
}


AosDataType::E 
AosCheckDatabaseExist::getDataType(
		AosRundata *rdata,
		AosDataRecordObj *record)
{
	return AosDataType::eBool;
}


bool
AosCheckDatabaseExist::getValue(
		AosRundata *rdata, 
		AosValueRslt &value, 
		AosDataRecordObj *record)
{
	AosGenFunc::getValue(rdata, 0, record);
	OmnString db_name = mValue.getStr(); 
	bool rslt = AosJqlStmtDatabase::checkDatabase(rdata, db_name);
	value.setBool(rslt);
	return true;
}


bool 
AosCheckDatabaseExist::syntaxCheck(
		const AosRundataPtr &rdata, 
		OmnString &errmsg)
{
	if (mSize != 1)
	{
		errmsg << "CheckDatabaseExist needs only 1 parameters.";
		return false;
	}

	return true;
}


AosJimoPtr
AosCheckDatabaseExist::cloneJimo() const
{
	try
	{
		return OmnNew AosCheckDatabaseExist(*this);
	}

	catch (...)
	{
		OmnAlarm << "Failed creating dataset" << enderr;
	}

	OmnShouldNeverComeHere;
	return 0;
}

