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
// 2014/10/26 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "GenericObjs/Schema/CreateDataProc.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "API/AosApi.h"
#include "ErrorMgr/ErrmsgId.h"
#include "Jimo/Ptrs.h"
#include "Rundata/Rundata.h"
#include "SEInterfaces/ExprObj.h"
#include "SEUtil/ParmNames.h"
#include "Util/OmnNew.h"
#include "XmlUtil/XmlTag.h"
#include "JQLParser/JQLParser.h"

extern "C"
{

AosJimoPtr AosCreateJimoFunc_AosMethodCreateDataProc_1(const AosRundataPtr &rdata, const int version)
{
	try
	{
		OmnScreen << "To create Jimo: " << endl;
		AosJimoPtr jimo = OmnNew AosMethodCreateDataProc(version);
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


AosMethodCreateDataProc::AosMethodCreateDataProc(const int version)
:
AosGenericMethod(version)
{
}


AosMethodCreateDataProc::~AosMethodCreateDataProc()
{
}


bool
AosMethodCreateDataProc::config(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &worker_doc,
		const AosXmlTagPtr &jimo_doc)
{
	return true;
}


AosJimoPtr 
AosMethodCreateDataProc::cloneJimo() const
{
	return OmnNew AosMethodCreateDataProc(*this);
}


bool 
AosMethodCreateDataProc::proc(
		AosRundata *rdata,
		const OmnString &proc_name, 
		const vector<AosGenericValueObjPtr> &parms)
{
	// This function creates a generic object. 
	// The JQL format:
	// 	create "data_proc" "proc-name" 
	// 	(
	// 	    'type':'compose',
	// 	    'name':'value',
	// 	    ...
	// 	    'name':'value',
	// 	);
	//
	// It creates a data proc. If the data proc already exists, it is an error.
	//
	// It converts it to an XML:
	// 	<xml ...>
	// 		<...>
	// 	</xml>

	if (proc_name == "")
	{
		AosLogError(rdata, true, AosErrmsgId::eMissingDataProcName) << enderr;
		return false;
	}

	AosDataRecordObjPtr record = AosCreateDataRecord(rdata, parms);
	OmnString type = record->getFieldValue(rdata, AOSPARM_TYPE);
	if (type == "")
	{
		AosLogError(rdata, true, AosErrmsgId::eMissingDataProcType) << enderr;
		return false;
	}

	if (type == "compose")
	{
		return createDataProcCompose(rdata, proc_name, records);
	}

	return true;
}


bool 
AosMethodCreateDataProc::createDataProcCompose(
		AosRundata *rdata,
		const OmnString &proc_name, 
		const AosDataRecordObjPtr &parms)
{
	// This function creates the XML for the data proc:
	// 	create "data_proc" "proc-name" 
	// 	(
	// 	    'type':'compose',
	// 	    'name':'value',
	// 	    'filter':'xxx',
	// 	    ...
	// 	    'name':'value',
	// 	);
	


}

