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
// 03/24/2013	Deleted by Ketty 
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_RlbTester_TesterTrans_RlbTesterDeleteFileTrans_h
#define Aos_RlbTester_TesterTrans_RlbTesterDeleteFileTrans_h

#include "TransUtil/CubicTrans.h"

class AosRlbTesterDeleteFileTrans : public AosCubicTrans
{

private:
	u64			mFileId;

public:
	AosRlbTesterDeleteFileTrans(const bool regflag);
	AosRlbTesterDeleteFileTrans(
			const u32 cube_id,
			const u64 file_id);
	~AosRlbTesterDeleteFileTrans();

	// Trans Interface	
	virtual bool serializeFrom(const AosBuffPtr &buff);
	virtual bool serializeTo(const AosBuffPtr &buff);
	virtual OmnString getStrType(){ return "eRlbTester_DeleteFile"; };
	virtual AosTransPtr clone();
	virtual bool proc();
	
};
#endif

