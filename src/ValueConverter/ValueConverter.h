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
// 2013/05/03 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_ValueConverter_ValueConverter_h
#define Aos_ValueConverter_ValueConverter_h

#include "Rundata/Ptrs.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"


class AosValueConverter : virtual public OmnRCObject
{
public:
	AosValueConverter();
	~AosValueConverter();

	virtual bool  run(const AosRundataPtr &rdata) = 0;
};
#endif

