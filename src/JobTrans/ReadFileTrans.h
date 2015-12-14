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
#ifndef Aos_JobTrans_ReadFileTrans_h
#define Aos_JobTrans_ReadFileTrans_h

#include "TransUtil/TaskTrans.h"

class AosReadFileTrans : virtual public AosTaskTrans
{
	u64				mFileId;
	OmnString 		mFileName;
	int64_t			mSeekPos;
	u32				mBytesToRead;

public:
	AosReadFileTrans(const bool regflag);
	AosReadFileTrans(
			const u64 &fileId,
			const int svr_id,
			const int64_t &seekPos,
			const u32 bytes_to_read);
	AosReadFileTrans(
			const OmnString &filename,
			const int svr_id,
			const int64_t &seekPos,
			const u32 bytes_to_read);
	~AosReadFileTrans();

	// Trans Interface	
	virtual bool serializeFrom(const AosBuffPtr &buff);
	virtual bool serializeTo(const AosBuffPtr &buff);
	virtual AosTransPtr clone();
	virtual bool proc();

};
#endif

