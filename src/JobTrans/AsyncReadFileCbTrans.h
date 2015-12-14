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
#ifndef Aos_JobTrans_AsyncReadFileCbTrans_h
#define Aos_JobTrans_AsyncReadFileCbTrans_h

#include "TransUtil/TaskTrans.h"
#include "SEInterfaces/AioCaller.h"

class AosAsyncReadFileCbTrans : virtual public AosTaskTrans
{

private:
	AosAioCaller::Parameter		mParm;
	int							mCallerId;
	AosBuffPtr					mBuff;

public:
	AosAsyncReadFileCbTrans(const bool regflag);
	AosAsyncReadFileCbTrans(
			const AosAioCaller::Parameter &parm,
			const int callerId,
			const AosBuffPtr &buff,
			const int svr_id,
			const u32 proc_id,
			const bool need_save,
			const bool need_resp);
	~AosAsyncReadFileCbTrans();

	// Trans Interface	
	virtual bool serializeFrom(const AosBuffPtr &buff);
	virtual bool serializeTo(const AosBuffPtr &buff);
	virtual AosTransPtr clone();
	virtual bool proc();
};
#endif
#endif 

