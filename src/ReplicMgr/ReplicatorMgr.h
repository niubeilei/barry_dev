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
// 2014/11/08	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_ReplicMgr_ReplicatorMgr_h
#define AOS_ReplicMgr_ReplicatorMgr_h

#include "SEInterfaces/ReplicatorMgrObj.h"
#include "Thread/ThreadedObj.h"


class AosReplicatorMgr : public AosReplicatorMgrObj, 
						 public OmnThreadedObj	
{
	OmnDefineRCObject;
	
private:
	
public:
	AosReplicatorMgr();
	~AosReplicatorMgr();

	// OmnThreadedObj interface
	virtual bool	threadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread);
	virtual bool	signal(const int threadLogicId);
};
#endif
