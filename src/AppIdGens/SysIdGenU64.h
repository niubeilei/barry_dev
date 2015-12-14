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
// 01/06/2013 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_SysIdGens_SysIdGenU64_h
#define AOS_SysIdGens_SysIdGenU64_h

#include "AppIdGens/AppIdGenU64.h"


class AosSysIdGenU64 : virtual public AosAppIdGenU64
{
private:
	OmnThreadPtr                mThread;

public:
	AosSysIdGenU64(
		const AosRundataPtr &rdata, 
		const OmnString &objid);
	AosSysIdGenU64(
		const AosRundataPtr &rdata,
		const OmnString &objid, 
		const u64 &block_size,
		const u64 &init_value,
		const u64 &max_id,
		const u64 &prefix);
	~AosSysIdGenU64();

	virtual OmnString getIILName() const;

};

#endif
