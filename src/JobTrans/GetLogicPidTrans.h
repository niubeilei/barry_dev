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
#ifndef Aos_JobTrans_GetLogicPidTrans_h
#define Aos_JobTrans_GetLogicPidTrans_h

#include "TransUtil/TaskTrans.h"

class AosGetLogicPidTrans : virtual public AosTaskTrans
{

private:
	u64		mTaskDocId;

public:
	AosGetLogicPidTrans(const bool regflag);
	AosGetLogicPidTrans(
			const u64 taskDocId,
			const int svr_id);
	~AosGetLogicPidTrans();

	// Trans Interface	
	virtual bool serializeFrom(const AosBuffPtr &buff);
	virtual bool serializeTo(const AosBuffPtr &buff);
	virtual AosTransPtr clone();
	virtual bool proc();

};
#endif

