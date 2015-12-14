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
// 2015/02/01 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "GenericObjs/RandDataGen/SysObjTracer.h"
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
AosJimoPtr AosCreateJimoFunc_AosSysObjTracer_1(const AosRundataPtr &rdata, const int version)
{
	try
	{
		OmnScreen << "To create Jimo: " << endl;
		AosJimoPtr jimo = OmnNew AosSysObjTracer(version);
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


AosSysObjTracer::AosSysObjTracer(const int version)
:
AosGenericMethod(version)
{
}


AosSysObjTracer::~AosSysObjTracer()
{
}


bool
AosSysObjTracer::config(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &worker_doc,
		const AosXmlTagPtr &jimo_doc)
{
	return true;
}


AosJimoPtr 
AosSysObjTracer::cloneJimo() const
{
	return OmnNew AosSysObjTracer(*this);
}


bool 
AosSysObjTracer::proc(
		AosRundata *rdata,
		const OmnString &data_name, 
		const vector<AosGenericValueObjPtr> &parms)
{
	// 	Update Tracer 
	// 	{
	// 	  
	// 	};

	AosDataRecordObjPtr record = AosCreateDataRecord(rdata, parms);
	OmnString jimo_name=record->getFieldValue(rdata,AOSPARM_DATAGEN_JIMO);
	bool flag = record->getFieldValue(rdata, "trace_doc");
	OmnTracer::mTraceDoc = flag;
	return true;
}

