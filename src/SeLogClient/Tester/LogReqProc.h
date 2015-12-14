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
// 2011/01/22	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Omn_SeLogClient_Tester_LogReqProc_h
#define Omn_SeLogClient_Tester_LogReqProc_h

#include "ReqProc/ReqProc.h"
#include "SeLogClient/Tester/Ptrs.h"
#include "SeLogClient/Tester/Reqids.h"
#include "Util/String.h"


class AosLogReqProc : public AosReqProc
{
	OmnDefineRCObject;

private:
	AosLogReqid::E	mId;
	OmnString		mReqidName;

public:
	AosLogReqProc(
			const OmnString &reqid, 
			const AosLogReqid::E id, 
			const bool regflag);
	AosLogReqProc();
	~AosLogReqProc() {}

	// LogReqProc Interface
	virtual bool proc(const AosRundataPtr &rdata) = 0;
	
	OmnString getReqidName() const {return mReqidName;}
	static AosLogReqProcPtr 	getProc(const AosLogReqid::E &id);
	static int 	getProcSize();

private:
	bool 	registerLogProc(const AosLogReqProcPtr &proc, OmnString &errmsg);
};
#endif

