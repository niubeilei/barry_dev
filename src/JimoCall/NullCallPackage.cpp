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
//
// Modification History:
// 2014/12/06 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "JimoCall/NullCallPackage.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "SEInterfaces/JimoCaller.h"


AosNullJimoCallPackage::AosNullJimoCallPackage(const int version)
{
}


bool 
AosNullJimoCallPackage::jimoCall(AosRundata *rdata, AosJimoCall &jimo_call)
{
	OmnShouldNeverComeHere;
	return false;
}


AosJimoPtr
AosNullJimoCallPackage::cloneJimo() const
{
	return OmnNew AosNullJimoCallPackage(mJimoVersion);
}

