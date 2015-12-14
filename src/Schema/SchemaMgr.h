////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// This type of IIL maintains a list of (string, docid) and is sorted
// based on the string value. 
//
// Modification History:
// 2013/12/07 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_Schema_SchemaMgr_h
#define AOS_Schema_SchemaMgr_h

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "SEInterfaces/SchemaMgrObj.h"

class AosSchemaMgr : public AosSchemaMgrObj
{

public:
	AosSchemaMgr();
	~AosSchemaMgr();

	virtual u64 getSchemaDocid(AosRundata *rdata, const OmnString &type);
};
#endif

