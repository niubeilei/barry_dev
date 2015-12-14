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
// 2014/07/25 Created by Ketty 
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_StatUtil_StatKeyCond_h
#define AOS_StatUtil_StatKeyCond_h


#include "Util/Opr.h"
#include "Util/String.h"

struct AosStatKeyCond
{
	OmnString 	mKeyFname;	
	AosOpr		mCondOpr;
	OmnString	mCondValue1;
	OmnString	mCondValue2;
};

#endif
