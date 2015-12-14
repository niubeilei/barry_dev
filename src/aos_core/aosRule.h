////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: aosRule.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef aos_core_aosRule_h
#define aos_core_aosRule_h

struct sk_buff;

enum
{
	eAosRuleType_BridgeRule,

	eAosMacCtlrRuleType_Filter,
	eAosMacCtlrRuleType_NoAos,
	eAosMacCtlrRuleType_MacAos,

	eAosRuleType_Idle			// Being returned to the slab
};

struct aosRuleHead
{
	struct aosRule *	mPrev;
	struct aosRule *	mNext;
	int					mType;
	int					mRuleId;
	int (*mProc)(struct aosRule *self, struct sk_buff *skb);
	int (*mDestructor)(struct aosRule *self);
};

struct aosRule
{
	struct aosRuleHead	mHead;
};

typedef int (*aosRuleProc)(struct aosRule *rule, struct sk_buff *skb);
typedef int (*aosRuleDestructor)(struct aosRule *rule);

extern struct aosRule *aosRule_constructor(aosRuleProc proc, 
					int type, 
					aosRuleDestructor destructor);
extern int aosRule_destructor(struct aosRule *self);
extern int aosRule_init(void);
extern int aosRule_getRuleName(int ruleid, char *name);
#endif

