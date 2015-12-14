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
// 05/17/2013	Created by Young Pan
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SysTrans_AskWheatherDeathTrans_h
#define Aos_SysTrans_AskWheatherDeathTrans_h

#include "API/AosApiR.h"
#include "TransUtil/Ptrs.h"
//#include "TransUtil/AsyncReqTrans.h"
#include "TransUtil/TaskTrans.h"
#include "Rundata/Rundata.h"

class AosAskWheatherDeathTrans : public AosTaskTrans //, public AosAsyncReqTrans
{
private:
	u32				mStartTime;
	int 			mDeathId;
	int				mServerId;

public:
	AosAskWheatherDeathTrans(const bool regflag);
	AosAskWheatherDeathTrans(
			const u32 start_time,
			const int death_id,
			const int ser_id, 
			const bool need_save,
			const bool need_resp);
	~AosAskWheatherDeathTrans();

	// Trans Interface	
	virtual bool serializeFrom(const AosBuffPtr &buff);
	virtual bool serializeTo(const AosBuffPtr &buff);
	virtual AosTransPtr clone();
	virtual bool proc();
	virtual bool respCallBack();

};
#endif


