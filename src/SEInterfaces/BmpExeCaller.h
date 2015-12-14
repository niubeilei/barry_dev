////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// Modification History:
// 2013/02/14 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_SEInterfaces_BmpExeCaller_h
#define AOS_SEInterfaces_BmpExeCaller_h

#include "Porting/TimeOfDay.h"
#include "Rundata/Ptrs.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"


class AosBmpExeCaller : virtual public OmnRCObject
{
private:
	u64		mTimestamp;

public:
	virtual void reqProcessed(const AosRundataPtr &rdata) = 0;

	void setCallTimestamp() 
	{
		mTimestamp = OmnGetTimestamp();
	}
};
#endif

