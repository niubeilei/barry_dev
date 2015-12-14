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
// 2013/11/01 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#if 0
#include "Dataset/Jimos/DatasetByQueryRslts.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "API/AosApi.h"
#include "Rundata/Rundata.h"
#include "XmlUtil/Ptrs.h"
#include "XmlUtil/XmlTag.h"

extern "C"
{

AosJimoPtr AosCreateJimoFunc_AosDatasetByQueryRslts(
 		const AosRundataPtr &rdata, 
 		const AosXmlTagPtr &sdoc) 
{
	try
	{
		OmnScreen << "To create Jimo: " << endl;
		AosJimoPtr jimo = OmnNew AosDatasetByQueryRslts(rdata, sdoc);
		aos_assert_rr(jimo, rdata, 0);
		rdata->setJimo(jimo);
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


AosDatasetByQueryRslts::AosDatasetByQueryRslts(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &def)
:
AosDataset(rdata, def)
{
	if (!config(rdata, def)) OmnThrowException(rdata->getErrmsg());
}


AosDatasetByQueryRslts::~AosDatasetByQueryRslts()
{
}


bool
AosDatasetByQueryRslts::config(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &def)
{
	if (!def) return true;
	return true;
}
#endif
