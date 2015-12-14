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
// 2013/03/01 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#if 0
#include "IILCache/CappletExample.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"


AosCappletExample::AosCappletExample(const bool flag)
:
AosCapplet(AosCappletId::eExample, flag)
{
}


AosCappletExample::~AosCappletExample()
{
}


AosCappletPtr 
AosCappletExample::clone()
{
	return OmnNew AosCappletExample();
}


bool 
AosCappletExample::run(
		const AosRundataPtr &rdata, 
		AosDatalet &parms,
		const AosDataletPtr &datalet)
{
	// It runs the capplet over 'datalet'. Parameters are passed over
	// 'parms'. Results may be saved in 'datalet', 'parms', or 'rdata'.
	// Note that capplets do not keep member data. If there are member
	// data, they should be saved in 'datalet'. Or in other word, 
	// 'datalet' + 'capplets' are objects. 
	aos_assert_rr(datalet, rdata, false);
	return true;
}

#endif
