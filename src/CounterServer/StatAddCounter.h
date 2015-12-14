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
// This is a utility to select docs.
//
// Modification History:
// 03/29/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_CounterServer_StatAddCounter_h
#define AOS_CounterServer_StatAddCounter_h

#include "CounterServer/StatProc.h"

class AosStatAddCounter : virtual public AosStatProc 
{
public:
	enum
	{
		eMaxSize = 300 + 1 + sizeof(u64)    // cname + '\0' + u64
	};
public:
	AosStatAddCounter(const u64 &iilid, const AosOrder::E order, const AosRundataPtr &rdata);
	~AosStatAddCounter() {}

	bool proc(const OmnString &counter_id, 
			const AosBuffPtr &buff, 
			AosVirCtnrSvr* vserver, 
			const AosRundataPtr &rdata);
};
#endif
