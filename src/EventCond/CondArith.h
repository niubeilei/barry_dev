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
// 11/18/2010	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_EventCond_CondArith_h
#define Aos_EventCond_CondArith_h

#include "EventCond/EventCond.h"
#include "EventCond/Ptrs.h"
#include "Rundata/Ptrs.h"
#include <vector>

class AosCondArith : virtual public AosEventCond
{
	OmnDefineRCObject;

public:
	AosCondArith();
	~AosCondArith();

	virtual bool evalCond(
			const AosXmlTagPtr &def, 
			bool rslt,
			const AosRundataPtr &rdata);
private:
	bool cmpCond(
			const OmnString &opr,
			const OmnString &v1,
			const OmnString &v2);

	bool cmpCond(
			const OmnString &opr,
			const int &v1,
			const int &v2);
};
#endif

