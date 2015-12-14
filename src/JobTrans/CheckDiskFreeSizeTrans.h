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
// 2014/10/10	Created by Ken Lee
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_JobTrans_CheckDiskFreeSizeTrans_h
#define Aos_JobTrans_CheckDiskFreeSizeTrans_h

#include "TransUtil/TaskTrans.h"

class AosCheckDiskFreeSizeTrans : virtual public AosTaskTrans
{

private:

public:
	AosCheckDiskFreeSizeTrans(const bool regflag);
	AosCheckDiskFreeSizeTrans(
			const int svr_id);
	~AosCheckDiskFreeSizeTrans();

	// Trans Interface	
	virtual bool serializeFrom(const AosBuffPtr &buff);
	virtual bool serializeTo(const AosBuffPtr &buff);
	virtual AosTransPtr clone();
	virtual bool proc();

};
#endif

