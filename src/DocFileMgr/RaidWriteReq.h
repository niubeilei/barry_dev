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
// Modification History:
// 05/013/2013 Created by Linda Lin 
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_DocFileMgr_RaidWriteReq_h
#define AOS_DocFileMgr_RaidWriteReq_h

#include "DocFileMgr/RaidFileReq.h"
#include "Rundata/Ptrs.h"
#include "SEInterfaces/Ptrs.h"
#include "SEInterfaces/AioCaller.h"
#include "StorageMgr/Ptrs.h"
#include "Thread/Sem.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"
#include "Util/Ptrs.h"

struct AosRaidWriteReq : virtual public AosRaidFileReq, public AosAioCaller 
{
private:

	vector<AosAioRequestPtr>		mRequest;
	AosBuffPtr						mBuff;
	OmnSemPtr						mSem;

public:
	AosRaidWriteReq(
			const char *buff,
			const int64_t &len,
			const OmnSemPtr &sem);

	AosRaidWriteReq();

	~AosRaidWriteReq();

	// AioCaller class interface
	virtual AosBuffPtr getIoBuff(const u64 &reqId, const int64_t &size AosMemoryCheckDecl);
	virtual void dataRead(const Parameter &parm, const AosBuffPtr &buff);
	virtual void readError(u64 &reqid, OmnString &errmsg);
	virtual void dataWrite(const u64 &reqid); 
	virtual void writeError(u64 &reqid, OmnString &errmsg);

	virtual u64 	getTotalReq(){return mRequest.size();}

	virtual bool 	addRequest(
						const u64 &reqid,
						const OmnFilePtr &ff,
						const int64_t &seek_pos,
						const int64_t &bytes_to_read);

	virtual bool	sendRequest();

	virtual void 	clear();
private:
	bool	sanityCheck();
};
#endif
