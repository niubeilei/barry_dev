////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
//
// Modification History:
// 2013/05/07 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "ActionDyn/CreateFiles.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "API/AosApi.h"
#include "DataCalcDyn/DataCalcNames.h"
#include "Rundata/Rundata.h"
#include "Util/ValueRslt.h"
#include "XmlUtil/Ptrs.h"
#include "XmlUtil/XmlTag.h"

extern "C"
{

AosDLLObjPtr AosCreateDLLActionCreateFiles(
 		const AosRundataPtr &rdata, 
 		const AosXmlTagPtr &sdoc, 
 		const OmnString &libname,
 		const OmnString &version) 
{
	rdata->setDLLObj(0);
	try
	{
		AosDLLObjPtr obj = OmnNew AosDLLActionCreateFiles(rdata, 0, 
				libname, "AosCreateDLLActionCreateFiles", version);
		aos_assert_rr(obj, rdata, 0);
		rdata->setDLLObj(obj);
		return obj;
	}

	catch (...)
	{
		AosSetErrorU(rdata, "Failed running DLLActionCreateFiles DLL function");
		return 0;
	}

	OmnShouldNeverComeHere;
	return 0;
}
}

AosDLLActionCreateFiles::AosDLLActionCreateFiles(
		const AosRundataPtr &rdata, 
		const AosXmlTagPtr &sdoc, 
		const OmnString &libname, 
		const OmnString &method,
		const OmnString &version)
:
AosDLLActionObj("create_files", libname, method, version)
{
	if (!config(rdata, sdoc))
	{
		OmnThrowException(rdata->getErrmsg());
		return;
	}
}


AosDLLActionCreateFiles::~AosDLLActionCreateFiles()
{
}


bool
AosDLLActionCreateFiles::config(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &def)
{
	return true;
}


bool 
AosDLLActionCreateFiles::serializeTo(AosBuffPtr &buff, const AosRundataPtr &rdata)
{
	OmnNotImplementedYet;
	return true;
}


bool 
AosDLLActionCreateFiles::serializeFrom(const AosBuffPtr &buff, const AosRundataPtr &rdata)
{
	OmnNotImplementedYet;
	return true;
}


bool 
AosDLLActionCreateFiles::abortAction(const AosRundataPtr &rdata)
{
	OmnScreen << "run(rdata)" << endl;
	return true;
}


bool 
AosDLLActionCreateFiles::run(const AosXmlTagPtr &def, const AosRundataPtr &rundata)
{
	// This function creates files. The files are defined in 'def'. 
	OmnNotImplementedYet;
	return true;
}


bool 
AosDLLActionCreateFiles::run(const AosValueRslt &value, const AosRundataPtr &rdata)
{
	OmnNotImplementedYet;
	return true;
}


bool 
AosDLLActionCreateFiles::run(const AosDataRecordObjPtr &record, const AosRundataPtr &rdata)
{
	OmnNotImplementedYet;
	return true;
}


bool 
AosDLLActionCreateFiles::run(const AosDataBlobObjPtr &blob, const AosRundataPtr &rdata)
{
	OmnNotImplementedYet;
	return true;
}


bool 
AosDLLActionCreateFiles::run(const char *data, const int len, const AosRundataPtr &rdata)
{
	OmnNotImplementedYet;
	return true;
}


bool 
AosDLLActionCreateFiles::run(const AosBuffPtr &buff, const AosRundataPtr &rdata)
{
	OmnNotImplementedYet;
	return true;
}


bool 
AosDLLActionCreateFiles::run(const AosTaskObjPtr &task, 
					const AosXmlTagPtr &sdoc,
					const AosTaskDataObjPtr &def, 
					const AosRundataPtr &rundata)
{
	OmnNotImplementedYet;
	return true;
}


