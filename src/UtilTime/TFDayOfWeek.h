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
#ifndef Omn_UtilTime_TFDayOfWeek_h
#define Omn_UtilTime_TFDayOfWeek_h

#include "UtilTime/TimeFormat.h"

class AosTFDayOfWeek : public AosTimeFormat
{
public:
	AosTFDayOfWeek(const bool regflag);
	~AosTFDayOfWeek();

	virtual int getDataLen() const {return sizeof(u32);}
};
#endif

