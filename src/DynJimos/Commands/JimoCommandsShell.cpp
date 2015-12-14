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
// 2013/06/18 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "DynJimos/Commands/JimoCommands.h"
#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "API/AosApi.h"
#include "Rundata/Rundata.h"
#include "XmlUtil/Ptrs.h"
#include "XmlUtil/XmlTag.h"
#include <string>

extern "C"
{

AosJimoPtr AosCreateJimoFunc_AosJimoCommands(
 		const AosRundataPtr &rdata, 
 		const AosXmlTagPtr &worker_doc, 
 		const AosXmlTagPtr &jimo_doc) 
{
	try
	{
		OmnScreen << "To create Jimo: " << endl;
		return OmnNew AosJimoCommands(rdata, worker_doc, jimo_doc);
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

