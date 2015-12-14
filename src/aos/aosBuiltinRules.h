////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: aosBuiltinRules.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef aos_aos_core_BuiltinRules_h
#define aos_aos_core_BuiltinRules_h

#include "aos_core/aosRule.h"
#include "aos/KernelEnum.h"

struct sk_buff;
struct aosRule;

struct aosBridgeRule
{
	struct aosRule *	mPrev;
	struct aosRule *	mNext;
	int					mType;
	int					mRuleId;
	int (*mProc)(struct aosRule *self, struct sk_buff *skb);
	int (*mDestructor)(struct aosRule *self);

	struct aosBridgeDef *	mBridge;
};

extern int aosBridgeRule_create(unsigned char ruleId,
                    char *names1, 
					int weight1,
                    char *names2, 
					int weight2);
extern int aosBridgeRule_proc(struct aosRule *self, struct sk_buff *skb);
extern int aosBridgeRule_destructor(struct aosRule *self);

#endif
