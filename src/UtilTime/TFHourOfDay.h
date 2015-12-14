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
// 08/12/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Omn_UtilTime_TFHourOfDay_h
#define Omn_UtilTime_TFHourOfDay_h

#include "UtilTime/TimeFormat.h"

class AosTFHourOfDay : public AosTimeFormat
{
public:
	AosTFHourOfDay(const bool regflag);
	~AosTFHourOfDay();

	virtual int getDataLen() const {return sizeof(u32);}
};
#endif

