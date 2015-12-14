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
#ifndef Aos_CubeComm_Tester_RawSETester_h
#define Aos_CubeComm_Tester_RawSETester_h

#include "Debug/Debug.h"
#include "JimoCall/JimoCaller.h"
#include "Tester/TestPkg.h"



class AosRawSETester : public OmnTestPkg,
					   public AosJimoCaller
{
private:
	int		mTries;

public:
	AosRawSETester();
	~AosRawSETester() {}

	virtual bool callSuccess(const AosRundataPtr &rdata, const i64 call_id);
	virtual bool callFailed(const AosRundataPtr &rdata, const i64 call_id);

	virtual bool		start();

private:
};
#endif

