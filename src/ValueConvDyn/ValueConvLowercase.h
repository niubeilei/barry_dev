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
// 2013/05/03 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_ValueConvDyn_ValueConvLowercase_h
#define Aos_ValueConvDyn_ValueConvLowercase_h

#include "ValueConverter/ValueConverter.h"
#include "ValueConverter/Ptrs.h"
#include "Rundata/Ptrs.h"

class AosValueConvLowercase : public AosValueConverter
{
	OmnDefineRCObject;

public:
	AosValueConvLowercase();
	~AosValueConvLowercase();

	virtual bool run(const AosRundataPtr &rdata);
};
#endif

