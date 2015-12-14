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
// 2014/08/06	Created By Ken Lee
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_JobTrans_AsyncReadFileTrans_h
#define Aos_JobTrans_AsyncReadFileTrans_h

#include "TransUtil/TaskTrans.h"
#include "SEInterfaces/AioCaller.h"

class AosAsyncReadFileTrans : virtual public AosTaskTrans, virtual public AosAioCaller
{

private:
	u64							mReqId;
	AosAioCallerPtr				mCaller;
	u64							mFileId;
	OmnString					mFileName;
	int64_t						mSeekPos;
	u32							mBytesToRead;

public:
	AosAsyncReadFileTrans(const bool regflag);
	AosAsyncReadFileTrans(
				const u64 &reqid,
				const AosAioCallerPtr &caller,
				const u64 &fileId,
				const int svr_id,
				const int64_t &seekPos,
				const u32 bytes_to_read);
	AosAsyncReadFileTrans(
				const u64 &reqid,
				const AosAioCallerPtr &caller,
				const OmnString &filename,
				const int svr_id,
				const int64_t &seekPos,
				const u32 bytes_to_read);
	~AosAsyncReadFileTrans();

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

