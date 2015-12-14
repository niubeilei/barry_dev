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
// 05/21/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "SEInterfaces/IILScannerObj.h"

#include "Alarm/Alarm.h"
#include "alarm_c/alarm.h"
#include "Rundata/Rundata.h"


AosIILScannerObjPtr AosIILScannerObj::smIILScannerObj;

AosIILScannerObjPtr 
AosIILScannerObj::createIILScannerStatic(
		const AosIILScannerListenerPtr &caller,
		const int seqno,
		const AosXmlTagPtr &def, 
		const AosRundataPtr &rdata)
{
	aos_assert_rr(smIILScannerObj, rdata, 0);
	return smIILScannerObj->createIILScanner(caller, seqno, def, rdata);
}


