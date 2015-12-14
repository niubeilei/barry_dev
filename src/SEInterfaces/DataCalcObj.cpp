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
// 2013/05/05 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "SEInterfaces/DataCalcObj.h"

#include "Alarm/Alarm.h"
#include "alarm_c/alarm.h"
#include "Rundata/Rundata.h"
#include "XmlUtil/XmlTag.h"



AosDataCalcObj::AosDataCalcObj(
		const OmnString &name, 
		const OmnString &libname, 
		const OmnString &method,
		const OmnString &version)
:
AosDLLObj(eDataCalc, libname, method, version),
mCalcName(name)
{
}


AosDataCalcObj::~AosDataCalcObj()
{
}


