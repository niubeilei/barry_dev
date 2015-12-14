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
// 06/15/2011	Created by Jozhi Peng
////////////////////////////////////////////////////////////////////////////
#ifndef Omn_MsgServer_MsgRequestProc_h
#define Omn_MsgServer_MsgRequestProc_h

#include "MsgServer/MsgReqids.h"
#include "MsgServer/Ptrs.h"
#include "Util/String.h"
#include "ReqProc/ReqProc.h"
#include "Thread/Thread.h"
#include "Thread/ThreadedObj.h"
#include <map>
using namespace std;

class AosMsgRequestProc : public OmnRCObject
{
	OmnDefineRCObject;
private:
	AosMsgReqid::E	mId;
	OmnString mReqidName;

public:
	AosMsgRequestProc(
			const OmnString &reqid, 
			const AosMsgReqid::E id, 
			const bool regflag);
	~AosMsgRequestProc() {}

	// MsgRequestProc Interface
	virtual bool proc(
			const AosXmlTagPtr &request,
			const AosRundataPtr &rdata) = 0;
	
	OmnString getReqidName() const {return mReqidName;}
	static AosMsgRequestProcPtr 	getProc(const OmnString &reqid);

private:
	bool 	registerMsgProc(const AosMsgRequestProcPtr &proc);
};
#endif

