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
// 2011/07/28	Created by Jackie zhao
////////////////////////////////////////////////////////////////////////////
#ifndef Omn_ReqProc_ProcRequest_h
#define Omn_ReqProc_ProcRequest_h

#include "ReqProc/ReqProc.h"
#include "ProcRequest/Ptrs.h"
#include "ProcRequest/Reqids.h"
#include "Util/String.h"

#include "ProcServer/ProcServer.h"

class AosProcRequest : public AosReqProc
{
	OmnDefineRCObject;

private:
	AosProcReqid::E	mId;
	OmnString		mReqidName;

public:
	AosProcRequest(
			const OmnString &reqid, 
			const AosProcReqid::E id, 
			const bool regflag);
	~AosProcRequest() {}

	// ProcRequest Interface
	virtual bool proc(const AosRundataPtr &rdata) = 0;
	virtual void setProcServer(const AosProcServerPtr &procserver) = 0;
	OmnString getReqidName() const {return mReqidName;}
	static AosProcRequestPtr 	getProc(const OmnString &reqid);

private:
	bool 	registerProcRequest(const AosProcRequestPtr &proc);
};
#endif

