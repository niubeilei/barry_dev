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
#ifndef Aos_JobTrans_GetFileListByDataCol_h
#define Aos_JobTrans_GetFileListByDataCol_h

#include "TransUtil/TaskTrans.h"

class AosGetFileListByDataCol : virtual public AosTaskTrans
{
private:
	OmnString 		mDataColId;
	u64				mJobDocid;

public:
	AosGetFileListByDataCol(const bool regflag);
	AosGetFileListByDataCol(
			const OmnString &datacol_id,
			const u64 &job_id,
			const int svr_id);

	~AosGetFileListByDataCol();

	// Trans Interface	
	virtual bool serializeFrom(const AosBuffPtr &buff);
	virtual bool serializeTo(const AosBuffPtr &buff);
	virtual AosTransPtr clone();
	virtual bool proc(); 

};
#endif

