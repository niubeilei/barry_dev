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
#if 0
#ifndef Aos_JobTrans_GetFileLengthByIdTrans_h
#define Aos_JobTrans_GetFileLengthByIdTrans_h

#include "TransUtil/TaskTrans.h"

class AosGetFileLengthByIdTrans : virtual public AosTaskTrans
{

private:
	u64				mFileId;

public:
	AosGetFileLengthByIdTrans(const bool regflag);
	AosGetFileLengthByIdTrans(
			const u64 &fileId,
			const int svr_id,
			const bool need_save,
			const bool need_resp);
	~AosGetFileLengthByIdTrans();

	// Trans Interface	
	virtual bool serializeFrom(const AosBuffPtr &buff);
	virtual bool serializeTo(const AosBuffPtr &buff);
	virtual AosTransPtr clone();
	virtual bool proc(); 

};
#endif
#endif

