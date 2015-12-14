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
// 02/19/2014	Creaded by Andy.Zhang 
////////////////////////////////////////////////////////////////////////////
#ifndef Omn_SEUtil_TimeTypes_h
#define Omn_SEUtil_TimeTypes_h

struct TimeTypes 
{
	enum Month
	{
		eInvalid,

		eJanuary,
		eFebruary,
		eMarch,
		eApril,
		eMay,
		eJune,
		eJuly,
		eAugust,
		eSeptember,
		eOctober,
		eNovember,
		eDecember,

		eMax
	};

	enum WeekDay
	{
		eInvalid,

		eSunday,
		eMonday,
		eTuesday,
		eWednesday,
		eThursday,
		eFriday,
		eSaturday,

		eMax
	}
};


#endif
