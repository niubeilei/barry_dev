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
// 07/28/2011	Created by Linda
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_JobTrans_AsyncGetFileLengthCbTrans_h
#define Aos_JobTrans_AsyncGetFileLengthCbTrans_h

#include "TransUtil/TaskTrans.h"

class AosAsyncGetFileLengthCbTrans : virtual public AosTaskTrans
{

private:
	int							mCallerId;
	int64_t						mLength;

public:
	AosAsyncGetFileLengthCbTrans(const bool regflag);
	AosAsyncGetFileLengthCbTrans(
			const int callerId,
			const int64_t &fileLen,
			const int svr_id);
	~AosAsyncGetFileLengthCbTrans();

	// Trans Interface	
	virtual bool serializeFrom(const AosBuffPtr &buff);
	virtual bool serializeTo(const AosBuffPtr &buff);
	virtual AosTransPtr clone();
	virtual bool proc();
};
#endif

