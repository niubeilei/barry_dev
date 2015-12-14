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
// 06/09/2007 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "Util/TimeStr.h"

#include "Util/StrParser.h"

bool aos_is_valid_timestr(const OmnString &str)
{
	// 
	// Time strings are:
	// 		HH:MM:SS
	//
	OmnStrParser1 parser(str, ":", false, false);

	OmnString w = parser.nextWord("");
	if (w.length() <= 0 || w.length() > 2) return false;

	const char *data = w.data();
	if (data[0] < '0' || data[0] > '9') return false;
	if (w.length() == 2 && (data[1] < '0' || data[1] > '9')) return false;

	int hour = atoi(data);
	if (hour < 0 || hour > 23) return false;

	for (int i=0; i<2; i++)
	{
		OmnString w = parser.nextWord("");
		if (w.length() <= 0 || w.length() > 2) return false;
	
		const char *data = w.data();
		if (data[0] < '0' || data[0] > '9') return false;
		if (w.length() == 2 && (data[1] < '0' || data[1] > '9')) return false;
	
		int d = atoi(data);
		if (d < 0 || hour > 59) return false;
	}

	if (!parser.finished()) return false;
	return true;
}

	

