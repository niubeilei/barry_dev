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
// 2014/07/21	Created by Ketty 
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_StatTrans_CreateStatMetaFileTrans_h
#define Aos_StatTrans_CreateStatMetaFileTrans_h

#include "TransUtil/StatTrans.h"

class AosCreateStatMetaFileTrans : virtual public AosStatTrans
{

private:
	u32			mCubeId;
	OmnString	mFilePrefix;

public:
	AosCreateStatMetaFileTrans(const bool regflag);
	AosCreateStatMetaFileTrans(
				const u32 cube_id,
				const OmnString &file_prefix);
	~AosCreateStatMetaFileTrans();

	// Trans Interface	
	virtual bool serializeFrom(const AosBuffPtr &buff);
	virtual bool serializeTo(const AosBuffPtr &buff);
	virtual AosTransPtr clone();
	virtual bool proc();

private:
	bool 	setErrResp();

};
#endif

