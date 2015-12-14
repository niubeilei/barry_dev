////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: Trans.h
// Description:
//	This is the super class for transactions.   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SEUtil_AsyncRespCaller_h
#define Aos_SEUtil_AsyncRespCaller_h

#include "TransBasic/Ptrs.h"


class AosAsyncRespCaller: virtual public OmnRCObject
{
public:
	virtual void callback(
	 		const AosTransPtr &trans, 
	 		const AosBuffPtr &resp, 
	 		const bool svr_death) = 0;
};

#endif

