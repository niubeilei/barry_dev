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
#ifndef Aos_RlbTester_TesterTrans_RlbTesterReadDataTrans_h
#define Aos_RlbTester_TesterTrans_RlbTesterReadDataTrans_h

#include "TransUtil/CubicTrans.h"

class AosRlbTesterReadDataTrans : public AosCubicTrans
{

private:
	u64			mFileId;
	u64			mOffset;
	u32			mDataLen;

public:
	AosRlbTesterReadDataTrans(const bool regflag);
	AosRlbTesterReadDataTrans(
			const u64 file_id,
			const u64 offset,
			const u32 data_len);
	~AosRlbTesterReadDataTrans();

	// Trans Interface	
	virtual bool serializeFrom(const AosBuffPtr &buff);
	virtual bool serializeTo(const AosBuffPtr &buff);
	virtual OmnString getStrType(){ return "eRlbTester_ReadData"; };
	virtual AosTransPtr clone();
	virtual bool proc();

};
#endif

