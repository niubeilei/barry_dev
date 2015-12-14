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
// 09/23/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_InmemCounter_InmemCounterU64_h
#define Aos_InmemCounter_InmemCounterU64_h

#include "InmemCounters/InmemCounter.h"


class AosInmemCounterU64 : public AosInmemCounter
{
private:

public:
	AosInmemCounterU64(const bool flag);
	~AosInmemCounterU64();

	virtual AosInmemCounterObjPtr clone(
						const AosXmlTagPtr &def, 
						const AosRundataPtr &rdata);
};
#endif

