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
// 2014/11/17 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "GenericObjs/Configuration/ConfigCreate.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "API/AosApi.h"
#include "Jimo/Ptrs.h"
#include "Rundata/Rundata.h"
#include "SEInterfaces/ExprObj.h"
#include "SEUtil/ParmNames.h"
#include "Util/OmnNew.h"
#include "XmlUtil/XmlTag.h"
#include "JQLParser/JQLParser.h"


extern "C"
{
AosJimoPtr AosCreateJimoFunc_AosMethodConfigCreate_1(const AosRundataPtr &rdata, const int version)
{
	try
	{
		OmnScreen << "To create Jimo: " << endl;
		AosJimoPtr jimo = OmnNew AosMethodConfigCreate(version);
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


AosMethodConfigCreate::AosMethodConfigCreate(const int version)
:
AosGenericMethod(version)
{
}


AosMethodConfigCreate::~AosMethodConfigCreate()
{
}


bool
AosMethodConfigCreate::config(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &worker_doc,
		const AosXmlTagPtr &jimo_doc)
{
	return true;
}


AosJimoPtr 
AosMethodConfigCreate::cloneJimo() const
{
	return OmnNew AosMethodConfigCreate(*this);
}


bool 
AosMethodConfigCreate::proc(
		AosRundata *rdata,
		const OmnString &data_name, 
		const vector<AosGenericValueObjPtr> &parms)
{
	// 	create "configuration" "name"
	// 	(
	// 		name: value,
	// 		name: value,
	// 		...
	// 	);

	AosDataRecordObjPtr record = AosCreateDataRecord(rdata, parms);
	aos_assert_rr(record, rdata, false);
	OmnNotImplementedYet;
	return false;
}


