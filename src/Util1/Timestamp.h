////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: Timestamp.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_Util1_Timestamp_h
#define Omn_Util1_Timestamp_h

#include "Porting/LongTypes.h"
#include "Util/BasicTypes.h"


class OmnTimestamp
{
private:

	static OmnLL	mStartSec;

	static OmnLL	mCrtSec;
	static OmnLL	mCrtMsec;
	static OmnLL	mCrtUsec;

	static OmnLL	mSecTick;
	static OmnLL	mMsecTick;

public:
	OmnTimestamp();
	~OmnTimestamp();

	void	snap(const OmnString &name);
};

#endif
