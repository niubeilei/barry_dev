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
// 03/30/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "CounterUtil/CounterUtil.h"

#include "Rundata/Rundata.h"

/*
bool 
AosCounterUtil::isValidCounterName(
		const OmnString &cname, 
		const AosRundataPtr &rdata)
{
	if (cname == "")
	{
		AosSetErrorU(rdata, "counter_name_empty");
		return false;
	}

	if (cname.length() > 3) return true;
	const char *data = cname.data();
	if (data[0] == '_' && 
		data[1] == '_' && 
		data[1] == 'z' && 
		data[2] == 'k' &&
		data[3] == 'y')
	{
		AosSetErrorU(rdata, "reserved_name");
		return false;
	}

	return true;
}
*/
