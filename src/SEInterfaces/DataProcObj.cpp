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
// 2013/05/04 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "SEInterfaces/DataProcObj.h"

#include "Alarm/Alarm.h"
#include "alarm_c/alarm.h"
#include "Rundata/Rundata.h"
#include "XmlUtil/XmlTag.h"


AosDataProcObjPtr	AosDataProcObj::smObject;

AosDataProcObjPtr 
AosDataProcObj::createDataProcStatic(
		const AosXmlTagPtr &def,
		const AosRundataPtr &rdata)
{
	aos_assert_rr(smObject, rdata, 0);
	return smObject->createDataProc(def, rdata);
}


