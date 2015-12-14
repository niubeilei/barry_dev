////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: aosSockSSL.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef aos_core_aosSockSSL_h
#define aos_core_aosSockSSL_h

#include "aos_core/aosSock.h"

struct aosSockSSL
{
	struct aosSockCommon	mCommon;

	struct aosSslContext	*mContext;
};

#endif

