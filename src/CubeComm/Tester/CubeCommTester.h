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
//
// Modification History:
// 2014/11/25 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_CubeComm_Tester_CubeCommTester_h
#define Aos_CubeComm_Tester_CubeCommTester_h

#include "Debug/Debug.h"
#include "CubeComm/Ptrs.h"
#include "Tester/TestPkg.h"
#include "UtilComm/CommListener.h"
#include "CubeComm/CubeComm.h"
#include "Rundata/Rundata.h"

class AosCubeCommTester : public OmnTestPkg, 
						  public OmnCommListener
{
	OmnDefineRCObject;
private:
	int				mTries;
	AosCubeCommPtr	mCubeComm;
    AosRundata      *mRundata;
	//u32             mEndpointId;
	//u32             mRemoteEndpointId;
	u32             mMsgReceived;
	OmnString       mValue;

public:
	AosCubeCommTester();
	~AosCubeCommTester() {}

	virtual bool		start();

	// OmnCommListener
	virtual bool		msgRead(const OmnConnBuffPtr &buff);
	virtual OmnString	getCommListenerName() const;
	virtual void 		readingFailed();
	virtual bool basicTest();
};
#endif

