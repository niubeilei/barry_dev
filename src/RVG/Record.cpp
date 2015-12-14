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
// 03/22/2008: Created by Chen Ding 
//
////////////////////////////////////////////////////////////////////////////
#include "RVG/Record.h"

#include "alarm/Alarm.h"
#include "RVG/CommonTypes.h"

AosRecord::AosRecord()
{
}


AosRecord::~AosRecord()
{
}


bool	
AosRecord::getValue(
		const AosTablePtr &table, 
		const OmnString &name, 
		AosValuePtr &value)
{
	aos_not_implemented_yet;
	return false;
}

