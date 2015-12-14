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
// It generates docs with N fields. 
// 	Field1:	func1(docid);
// 	Field2:	func2(docid);
// 	Field3:	func3(docid);
// 	Field4:	func4(docid);
// 	Field5:	func5(docid);
//
// This means that given a docid, we can calculate the values for Field1, 
// Field2, ..., Field5. 
//
// When querying, it randomly generates a number of conditions. 
// 1. Validating Docids
//    The doc must meet the conditions. Otherwise, it is an error.
// 2. Check Missing Docs
//    For the first docid, it needs check all values before the queried
//    doc do not meet the conditions.
//    For the next docid, any docs between the current one and the next
//    one shall not meet the conditions.
//
// Modification History:
// 2014/05/01 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "DataGen/DataForQuery.h"

#include "API/AosApi.h"
#include "DocClient/DocClient.h"
#include "DocClient/DocidMgr.h"
#include "Rundata/Rundata.h"
#include "XmlUtil/XmlTag.h"



extern "C"
{

AosJimoPtr AosCreateJimoFunc_AosDataForQuery_1(const AosRundataPtr &rdata, const int version)
{
	try
	{
		OmnScreen << "To create Jimo: " << endl;
		AosJimoPtr jimo = OmnNew AosDataForQuery(version);
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


AosDataForQuery::AosDataForQuery(const int version)
:
AosJimo(AosJimoType::eDataGen, version)
{
}


AosDataForQuery::~AosDataForQuery()
{
}


bool
AosDataForQuery::config(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &worker_doc,
		const AosXmlTagPtr &jimo_doc) 
{
	// 	<conf ..>
	// 		<recordset .../>
	// 		<schema .../>	
	// 		<data_scanner .../>	
	// 	</conf>
	return true;
}


