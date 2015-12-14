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
// 05/10/2013	Created by Young Pan
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_JobTrans_RemoteCmdTrans_h
#define Aos_JobTrans_RemoteCmdTrans_h

#include "TransUtil/TaskTrans.h"
//#include "TransUtil/AsyncReqTrans.h"
#include "CommandProc/Ptrs.h"

class AosRemoteCmdTrans : virtual public AosTaskTrans
{

private:
	OmnString			mCmd;

public:
	AosRemoteCmdTrans(const bool regflag);
	AosRemoteCmdTrans(
			const OmnString &buff,
			const int svr_id);
	~AosRemoteCmdTrans();

	// Trans Interface	
	virtual bool serializeFrom(const AosBuffPtr &buff);
	virtual bool serializeTo(const AosBuffPtr &buff);
	virtual AosTransPtr clone();  
	virtual bool proc();

};
#endif

