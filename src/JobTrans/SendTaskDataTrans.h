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
#if 0
#ifndef Aos_JobTrans_SendTaskDataTrans_h
#define Aos_JobTrans_SendTaskDataTrans_h

#include "TransUtil/TaskTrans.h"

class AosSendTaskDataTrans : virtual public AosTaskTrans
{

private:
	u64 		mTaskDocId;
	OmnString	mDataId;
	AosBuffPtr  mConfBuff; 
	AosBuffPtr  mDataBuff; 

public:
	AosSendTaskDataTrans(const bool regflag);
	AosSendTaskDataTrans(
			const int svr_id,
			const u32 to_proc_id,
			const u64 taskDocId,
			const OmnString dataId,
			const AosBuffPtr &confBuff,
			const AosBuffPtr &dataBuff);

	~AosSendTaskDataTrans();

	// Trans Interface	
	virtual bool serializeFrom(const AosBuffPtr &buff);
	virtual bool serializeTo(const AosBuffPtr &buff);
	virtual AosTransPtr clone();
	virtual bool proc();

};
#endif

#endif
