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
# if 0
#ifndef Aos_JobTrans_AsyncReadFileByFileNameTrans_h
#define Aos_JobTrans_AsyncReadFileByFileNameTrans_h

#include "TransUtil/TaskTrans.h"
#include "SEInterfaces/AioCaller.h"

class AosAsyncReadFileByFileNameTrans : virtual public AosTaskTrans, virtual public AosAioCaller
{

private:
	u64							mReqId;
	AosAioCallerPtr				mCaller;
	OmnString					mFileName;
	int64_t						mSeekPos;
	u32							mBytesToRead;

public:
	AosAsyncReadFileByFileNameTrans(const bool regflag);
	AosAsyncReadFileByFileNameTrans(
				const u64 &reqid,
				const AosAioCallerPtr &caller,
				const OmnString &filename,
				const int64_t &seekPos,
				const u32 bytes_to_read,
				const int svr_id,
				const bool need_save,
				const bool need_resp);
	~AosAsyncReadFileByFileNameTrans();

	// Trans Interface	
	virtual bool serializeFrom(const AosBuffPtr &buff);
	virtual bool serializeTo(const AosBuffPtr &buff);
	virtual AosTransPtr clone();
	virtual bool proc();
	virtual bool respCallBack();

	//AioCaller Interface
	virtual AosBuffPtr getIoBuff(const u64 &reqId, const int64_t &size AosMemoryCheckDecl);
	virtual void dataRead(const AosAioCaller::Parameter &parm, const AosBuffPtr &buff);
	virtual void readError(u64 &reqid, OmnString &errmsg);
	virtual void dataWrite(const u64 &reqid);
	virtual void writeError(u64 &reqid, OmnString &errmsg);
};
#endif
#endif

