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
// 03/24/2013	Created by Ketty 
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_RlbTester_TesterTrans_RlbTesterCreateFileTrans_h
#define Aos_RlbTester_TesterTrans_RlbTesterCreateFileTrans_h

#include "TransUtil/CubicTrans.h"

class AosRlbTesterCreateFileTrans : public AosCubicTrans
{

private:
	u32			mSiteId;
	u64			mRequestedSize;

public:
	AosRlbTesterCreateFileTrans(const bool regflag);
	AosRlbTesterCreateFileTrans(
			const u32 cube_id,
			const u32 site_id,
			const u64 requested_size);
	~AosRlbTesterCreateFileTrans();

	// Trans Interface	
	virtual bool serializeFrom(const AosBuffPtr &buff);
	virtual bool serializeTo(const AosBuffPtr &buff);
	virtual OmnString getStrType(){ return "eRlbTester_CreateFile"; };
	virtual AosTransPtr clone();
	virtual bool proc();
	virtual bool timeoutProc();
	
};
#endif

