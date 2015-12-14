////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: aosSock.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef aos_core_aosSock_h
#define aos_core_aosSock_h

#include <KernelSimu/types.h>

struct aosSockData
{
	int			type;
	void *		data;
};

struct aosSockCommon
{
	struct sock*			mSock;
	struct aosSockBridge	*mBridge;
};


struct aosSock
{
	struct aosSockCommon		mCommon;
// #define mSock mComm.mSock
// #define mBridge mCommon.mBridge
};

extern int aosSock_release(struct sock *sk);

#endif

