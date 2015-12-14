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
// 2015/11/17	Created by Barry
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_JobTrans_CreateDataReceiver_h
#define Aos_JobTrans_CreateDataReceiver_h

#include "TransUtil/TaskTrans.h"


class AosCreateDataReceiver : virtual public AosTaskTrans
{
private:
	OmnString 		mDataColId;
	OmnString		mConfig;

public:
	AosCreateDataReceiver(const bool regflag);
	AosCreateDataReceiver(
			const OmnString &datacol_id,
			const OmnString &conf,
			const int svr_id);
	~AosCreateDataReceiver();

	// Trans Interface	
	virtual bool serializeFrom(const AosBuffPtr &buff);
	virtual bool serializeTo(const AosBuffPtr &buff);
	virtual AosTransPtr clone();
	virtual bool proc(); 

};
#endif

