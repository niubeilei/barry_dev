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
// 2014/09/12 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "Funcs/DummyFunc.h"

#include "API/AosApi.h"
#include "Rundata/Rundata.h"
#include "SEInterfaces/ExprObj.h"
#include "XmlUtil/XmlTag.h"

AosDummyFunc::AosDummyFunc()
:
AosGenFunc("DummyFunc", 1)
{
}


AosDummyFunc::~AosDummyFunc()
{
}


bool
AosDummyFunc::config(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &worker_doc,
		const AosXmlTagPtr &jimo_doc) 
{
	return true;
}


bool
AosDummyFunc::getValue(
		AosRundata *rdata, 
		AosValueRslt &value, 
		AosDataRecordObj *record)
{
	return true;
}

bool 
AosDummyFunc::syntaxCheck(
		const AosRundataPtr &rdata, 
		OmnString &errmsg)
{
	return true;
}

AosJimoPtr
AosDummyFunc::cloneJimo()  const
{
	return OmnNew AosDummyFunc(*this);
}

