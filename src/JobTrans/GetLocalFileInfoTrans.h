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
// 2014/08/15	Created by Ken Lee
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_JobTrans_GetLocalFileInfoTrans_h
#define AOS_JobTrans_GetLocalFileInfoTrans_h

#include "TransUtil/TaskTrans.h"


class AosGetLocalFileInfoTrans : virtual public AosTaskTrans
{
	u64		mFileId;

public:
	AosGetLocalFileInfoTrans(const bool regflag);
	AosGetLocalFileInfoTrans(
			const int svr_id,
			const u64 &fileId);
	~AosGetLocalFileInfoTrans();

	// Trans Interface	
	virtual bool serializeFrom(const AosBuffPtr &buff);
	virtual bool serializeTo(const AosBuffPtr &buff);
	virtual AosTransPtr clone();
	virtual bool proc();

};
#endif

