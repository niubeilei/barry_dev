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
// 06/15/2011	Created by Brian Zhang
////////////////////////////////////////////////////////////////////////////
#ifndef Omn_ReqProc_ShmReqProc_h
#define Omn_ReqProc_ShmReqProc_h

#include "ReqProc/ReqProc.h"
#include "Rundata/Rundata.h"
#include "ShortMsgSvr/Ptrs.h"
#include "ShortMsgUtil/ShmReqids.h"
#include "Util/String.h"


class AosShmReqProc : public AosReqProc
{
	OmnDefineRCObject;

private:
	AosShmReqid::E	mId;
	OmnString		mReqidName;

public:
	
	AosShmReqProc(
			const OmnString &reqid, 
			const AosShmReqid::E id, 
			const bool regflag);
	~AosShmReqProc(){};

	OmnString 		getReqidName() const {return mReqidName;}
	static AosShmReqProcPtr 	getProc(const OmnString &reqid);
    void 			createLog(
						const OmnString &receiver, 
						const OmnString &msg, 
						const AosRundataPtr &rdata);
private:
	bool 	registerSeProc(const AosShmReqProcPtr &proc);
};
#endif

