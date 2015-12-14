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
// 05/21/2011	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_LogSvrLogProc_h
#define Aos_LogSvrLogProc_h

#include "LogSvr/LogProcId.h"
#include "LogSvr/Ptrs.h"
#include "Rundata/Ptrs.h"
#include "Util/String.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"


class AosLogProc : virtual public OmnRCObject
{
	OmnDefineRCObject;

private:
	AosLogProcId::E		mProcId;
	OmnString			mProcName;

public:
	AosLogProc(
			const OmnString &reqid, 
			const AosLogProcId::E id, 
			const bool regflag);
	~AosLogProc() {}

	// LogProc Interface
	virtual bool procReq(const AosRundataPtr &rdata) = 0;
	
	OmnString getProcName() const {return mProcName;}
	static AosLogProcPtr getProc(const OmnString &reqid);

private:
	bool 	registerProc(const AosLogProcPtr &proc);
};
#endif

