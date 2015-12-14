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
#ifndef Aos_JobTrans_WriteFileTrans_h
#define Aos_JobTrans_WriteFileTrans_h

#include "TransUtil/TaskTrans.h"

class AosWriteFileTrans : virtual public AosTaskTrans
{
	OmnString 		mFileName;
	int64_t			mSeekPos;
	AosBuffPtr		mBuff;

public:
	AosWriteFileTrans(const bool regflag);
	AosWriteFileTrans(
			const OmnString &filename,
			const int svr_id,
			const int64_t &seekPos,
			const AosBuffPtr &buff);
	~AosWriteFileTrans();

	// Trans Interface	
	virtual bool serializeFrom(const AosBuffPtr &buff);
	virtual bool serializeTo(const AosBuffPtr &buff);
	virtual AosTransPtr clone();
	virtual bool proc();

};
#endif

