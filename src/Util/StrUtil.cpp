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
// 01/14/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "Util/StrUtil.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"


int 
AosStrUtil::selectValue(
		const OmnString &value, 
		const int min_len,
		const vector<OmnString> &values, 
		const u32 num)
{
	if (value.length() < min_len) return -1;
	aos_assert_r(values.size() == num, -1);

	const char c1 = value.getChar(0);
	const char c2 = value.getChar(1);
	const char c3 = value.getChar(2);
	for (u32 i=0; i<values.size(); i++) 
	{
		if (c1 == values[i].getChar(0) && 
			c2 == values[i].getChar(1) &&
			c3 == values[i].getChar(2))
		{
			if (value == values[i]) return i;
		}
	}
	return -1;
}

OmnString 
AosStrUtil::cdata(const OmnString &value)
{
	OmnString rslt = "<![CDATA[";
	rslt << value << "]]>";
	return rslt;
}
