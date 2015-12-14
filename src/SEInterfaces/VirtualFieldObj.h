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
// 2014/11/16 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SEInterfaces_VirtualFieldObj_h
#define Aos_SEInterfaces_VirtualFieldObj_h

#include "Jimo/Jimo.h"
#include "Rundata/Ptrs.h"
#include "Util/Buff.h"
#include "Util/ValueRslt.h"

class AosDataRecordObj;

class AosVirtualFieldObj : public AosJimo
{
protected:

public:
	AosVirtualFieldObj(const int version);
	~AosVirtualFieldObj();

	virtual bool getFieldValue(
							AosRundata *rdata, 
							AosDataRecordObj *record, 
							AosValueRslt &value) = 0;
};

#endif
