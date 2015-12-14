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
// 2013/08/30 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SEInterfaces_BitmapInstrRunner_h
#define Aos_SEInterfaces_BitmapInstrRunner_h

#include "BitmapTrans/Ptrs.h"
#include "Jimo/Jimo.h"
#include "Rundata/Ptrs.h"
#include "Thread/Ptrs.h"
#include "Thread/ThreadedObj.h"
#include "Util/Ptrs.h"
#include <queue>
using namespace std;

class AosBitmapInstrRunnerObj : public AosJimo
{
	OmnDefineRCObject;

public:
	AosBitmapInstrRunnerObj(const int version);
	~AosBitmapInstrRunnerObj();

	static bool addTransStatic(const AosTransSendInstrsPtr &trans);
	virtual bool addTrans(const AosTransSendInstrsPtr &trans) = 0;
	static bool AosCreateBitmapInstrRunnerJimoDoc(const AosRundataPtr &rdata);
};
#endif


