////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
//
// Modification History:
// 08/05/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_TaskTrans_TaskTransTwoPhase_h
#define Aos_TaskTrans_TaskTransTwoPhase_h

#include "SEInterfaces/Ptrs.h"
#include "TaskTrans/TaskTrans.h"
#include <vector>
using namespace std;

class AosTaskTransTwoPhase : virtual public AosTaskTrans
{
	OmnDefineRCObject;

private:

public:
	AosTaskTransTwoPhase(const bool flag);
	AosTaskTransTwoPhase(const AosXmlTagPtr &def, const AosRundataPtr &rdata);
	~AosTaskTransTwoPhase();

	virtual bool serializeTo(AosBuffPtr &buff, const AosRundataPtr &rdata);
	virtual bool serializeFrom(const AosBuffPtr &buff, const AosRundataPtr &rdata);
	virtual int getProgress() const;
	virtual OmnString toString() const;

private:
	bool config(const AosXmlTagPtr &def, const AosRundataPtr &rdata);
};
#endif

