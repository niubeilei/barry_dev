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
#include "Funcs/Jimos/ExecuteJQL.h"

#include "API/AosApi.h"
#include "Rundata/Rundata.h"
#include "SEInterfaces/ExprObj.h"
#include "XmlUtil/XmlTag.h"
#include "JQLParser/JQLParser.h"

#include <cmath>

extern AosJQLParser gAosJQLParser; 

extern "C"
{

AosJimoPtr AosCreateJimoFunc_AosExecuteJQL_1(const AosRundataPtr &rdata, const int version)
{
	try
	{
		OmnScreen << "To create Jimo: " << endl;
		AosJimoPtr jimo = OmnNew AosExecuteJQL(version);
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


AosExecuteJQL::AosExecuteJQL(const int version)
:
AosGenFunc("ExecuteJQLing", version)
{
}

AosExecuteJQL::AosExecuteJQL()
:
AosGenFunc("ExecuteJQLing", 1)
{
}



AosExecuteJQL::AosExecuteJQL(const AosExecuteJQL &rhs)
:
AosGenFunc(rhs)
{
}


AosExecuteJQL::~AosExecuteJQL()
{
}


bool
AosExecuteJQL::config(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &worker_doc,
		const AosXmlTagPtr &jimo_doc) 
{
	return true;
}


AosDataType::E 
AosExecuteJQL::getDataType(
		AosRundata *rdata, 
		AosDataRecordObj *record)
{
	return AosDataType::eString;
}


bool
AosExecuteJQL::getValue(
		AosRundata *rdata, 
		AosValueRslt &value, 
		AosDataRecordObj *record)
{
	if (!AosGenFunc::getValue(rdata, 0, record)) return false;
	OmnString var_name = mValue.getStr();
	OmnString statments = ""; //getVar(var_name);
	statments << var_name << ";";

	OmnString errmsg;
	AosJqlStatementPtr stmts = AosParseStatement(statments, errmsg, rdata); 
	if (!stmts) 
	{
		AosLogError(rdata, true, "failed_parsing_statement")
			<< enderr;
		return false;
	}
	stmts->run(rdata, 0);
	return true;
}

bool 
AosExecuteJQL::syntaxCheck(
		const AosRundataPtr &rdata, 
		OmnString &errmsg)
{
	//check if the number of arguments >= 2
	if (mSize != 1)
	{
		errmsg << "ExecuteJQL need 1 parameter.";
		return false;
	}

	//need to check if each param is numbers later on

	return true;
}

AosJimoPtr
AosExecuteJQL::cloneJimo()  const
{
	try
	{
		return OmnNew AosExecuteJQL(*this);
	}

	catch (...)
	{
		OmnAlarm << "Failed clone Jimo" << enderr;
	}

	OmnShouldNeverComeHere;
	return 0;
}

