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
#ifndef Aos_JobTrans_ProcMsgTrans_h
#define Aos_JobTrans_ProcMsgTrans_h

#include "TransUtil/TaskTrans.h"

class AosProcMsgTrans : virtual public AosTaskTrans
{

private:
	OmnString	mContents;

public:
	AosProcMsgTrans(const bool regflag);
	AosProcMsgTrans(
			const OmnString &contents,
			const int svr_id,
			const u32 to_proc_id,
			const bool need_save);
	~AosProcMsgTrans();

	// Trans Interface	
	virtual bool serializeFrom(const AosBuffPtr &buff);
	virtual bool serializeTo(const AosBuffPtr &buff);
	virtual AosTransPtr clone();
	virtual bool proc();

};
#endif
