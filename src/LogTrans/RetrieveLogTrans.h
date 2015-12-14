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
// 03/20/2013	Created by Ketty 
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_LogTrans_RetrieveLogTrans_h
#define Aos_LogTrans_RetrieveLogTrans_h

#include "TransUtil/CubicTrans.h"

class AosRetrieveLogTrans : virtual public AosCubicTrans
{

private:
	u64			mLogId;

public:
	AosRetrieveLogTrans(const bool regflag);
	AosRetrieveLogTrans(
		const u64	logid,
		const bool need_save,
		const bool need_resp);
	~AosRetrieveLogTrans();

	// Trans Interface	
	virtual bool serializeFrom(const AosBuffPtr &buff);
	virtual bool serializeTo(const AosBuffPtr &buff);
	virtual AosTransPtr clone();
	virtual bool proc();

};
#endif

