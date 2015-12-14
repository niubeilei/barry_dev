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
#ifndef AOS_CounterServer_StatSetCounter_h
#define AOS_CounterServer_StatSetCounter_h

#include "CounterServer/StatProc.h"

class AosStatSetCounter : public AosStatProc 
{
public:
	AosStatSetCounter(const u64 &iilid, const AosRundataPtr &rdata);
	~AosStatSetCounter() {}

	bool proc(const OmnString &counter_id, 
			const AosBuffPtr &buff, 
			AosVirCtnrSvr* vserver, 
			const AosRundataPtr &rdata);

private:
	bool setValue(
			const OmnString &name,
			const int64_t &vvv,
			const AosRundataPtr &rdata);

	bool setValue(
			const OmnString &name,
			const int64_t &vvv,
			bool &added,
			const AosRundataPtr &rdata);
					
};
#endif
