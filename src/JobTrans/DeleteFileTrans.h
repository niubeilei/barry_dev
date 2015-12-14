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
#ifndef Aos_JobTrans_DeleteFileTrans_h
#define Aos_JobTrans_DeleteFileTrans_h

#include "TransUtil/TaskTrans.h"

class AosDeleteFileTrans : virtual public AosTaskTrans
{
	u64			mFileId;
	OmnString	mFileName;

public:
	AosDeleteFileTrans(const bool regflag);
	AosDeleteFileTrans(
			const u64 &fileId,
			const int svr_id);
	AosDeleteFileTrans(
			const OmnString &filename,
			const int svr_id);
	~AosDeleteFileTrans();

	// Trans Interface	
	virtual bool serializeFrom(const AosBuffPtr &buff);
	virtual bool serializeTo(const AosBuffPtr &buff);
	virtual AosTransPtr clone();
	virtual bool proc();

};
#endif

