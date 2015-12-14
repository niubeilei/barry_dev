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
#ifndef Aos_TaskTrans_TaskTransOnePhase_h
#define Aos_TaskTrans_TaskTransOnePhase_h

#include "SEInterfaces/Ptrs.h"
#include "TaskTrans/TaskTrans.h"
#include <vector>
using namespace std;

class AosTaskTransOnePhase : virtual public AosTaskTrans
{
	OmnDefineRCObject;

private:

public:
	AosTaskTransOnePhase(const bool flag);
	AosTaskTransOnePhase(const AosXmlTagPtr &def, const AosRundataPtr &rdata);
	~AosTaskTransOnePhase();

	virtual bool serializeTo(AosBuffPtr &buff, const AosRundataPtr &rdata);
	virtual bool serializeFrom(const AosBuffPtr &buff, const AosRundataPtr &rdata);
	virtual OmnString toString() const;

private:
	bool config(const AosXmlTagPtr &def, const AosRundataPtr &rdata);
};
#endif

