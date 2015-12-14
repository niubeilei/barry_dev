////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//	
// Description:
//
// Modification History:
// 2014/01/28 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_Dataset_Jimos_PassDataSync_h
#define Aos_Dataset_Jimos_PassDataSync_h

#include "DataSync/DataSync.h"
#include "Thread/ThreadedObj.h"
#include "Thread/Ptrs.h"

#include <queue>
class AosPassDataSync : public AosDataSync
{
	OmnDefineRCObject;

private:

public:
	AosPassDataSync(const int version);
	~AosPassDataSync();
	

private:
};
#endif

