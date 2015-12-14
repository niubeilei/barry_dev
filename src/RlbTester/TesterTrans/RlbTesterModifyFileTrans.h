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
// 03/24/2013	Modifyd by Ketty 
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_RlbTester_TesterTrans_RlbTesterModifyFileTrans_h
#define Aos_RlbTester_TesterTrans_RlbTesterModifyFileTrans_h

#include "TransUtil/CubicTrans.h"

class AosRlbTesterModifyFileTrans : public AosCubicTrans
{

private:
	u64			mFileId;
	u64			mOffset;
	AosBuffPtr  mData;

public:
	AosRlbTesterModifyFileTrans(const bool regflag);
	AosRlbTesterModifyFileTrans(
		const u32 cube_id,
		const u64 file_id,
		const u64 offset,
		const AosBuffPtr &data);
	~AosRlbTesterModifyFileTrans();

	// Trans Interface	
	virtual bool serializeFrom(const AosBuffPtr &buff);
	virtual bool serializeTo(const AosBuffPtr &buff);
	virtual OmnString getStrType(){ return "eRlbTester_ModifyFile"; };
	virtual AosTransPtr clone();
	virtual bool proc();
	
	virtual bool timeoutProc();
	
};
#endif

