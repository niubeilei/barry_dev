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
// 2013/08/28 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_DataReceiver_DataReceiver_h
#define Aos_DataReceiver_DataReceiver_h

#include "SEInterfaces/DataReceiverObj.h"
#include "SEInterfaces/Ptrs.h"

#include <queue>

class AosDataReceiver : public AosDataReceiverObj
{
private:
	queue<AosBuffPtr>		mBuffs;
	OmnMutexPtr				mLock;

public:
	AosDataReceiver();
	AosDataReceiver(const u32 version);
	virtual ~AosDataReceiver();

	// AosJimo Interface
	virtual AosJimoPtr cloneJimo()  const;

	static AosDataReceiverObjPtr createDataReceiver(
			const AosXmlTagPtr &conf,
			const AosRundataPtr &rdata);
	virtual bool dataReceiver(const AosBuffPtr &buff);
	bool	finishDataReceiver(const AosRundataPtr &rdata);


};
#endif

