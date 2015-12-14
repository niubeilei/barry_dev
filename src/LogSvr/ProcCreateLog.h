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
#ifndef Aos_LogSvr_ProcCreateLog_h
#define Aos_LogSvr_ProcCreateLog_h

#include "LogSvr/LogProc.h"
#include "Util/String.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"


class AosProcCreateLog : public AosLogProc
{
private:

public:
	AosProcCreateLog(
			const OmnString &reqid, 
			const AosLogProcId::E id, 
			const bool regflag);
	~AosProcCreateLog() {}

	// ProcCreateLog Interface
	virtual bool procReq(const AosRundataPtr &rdata);
};
#endif

