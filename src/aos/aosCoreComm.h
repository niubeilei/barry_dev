////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: aosCoreComm.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef aos_core_aosCoreComm_h
#define aos_core_aosCoreComm_h

#include "Porting/LongTypes.h"

struct aosCharPtree;

extern struct aosCharPtree *aosCliTree;

enum 
{
	eAosMethod_Invalid,
	eAosMethod_None,
	eAosMethod_WRR,
	eAosMethod_RR,
};


struct aosPktStat
{
	OmnUint64	mTotalPkts;
	OmnUint64 	mTotalBytes;
	OmnUint64	mErrorPkts;
	OmnUint64	mErrorBytes;
};

extern int aosCore_moduleInit(void);
extern int aosPktStat_reset(struct aosPktStat *self);
extern int aosCoreInit(char *errmsg, const int);

#endif
