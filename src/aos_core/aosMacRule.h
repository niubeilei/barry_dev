////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: aosMacRule.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef aos_core_aosMacRule_h
#define aos_core_aosMacRule_h

#include "aos_core/aosRule.h"


struct aosMacRule
{
	struct aosRuleHead	 mHead;
	unsigned char		 mMac[6];
};


struct sk_buff;

extern int aosMacRule_init(void);
extern struct aosMacRule * aosMacRule_constructor(aosRuleProc proc, 
					   int type, 
					   aosRuleDestructor destructor);
#endif

