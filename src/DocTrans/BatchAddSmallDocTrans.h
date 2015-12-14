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
// 10/08/2013	Created by Young
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_DocTrans_BatchAddSmallDocTrans_h
#define Aos_DocTrans_BatchAddSmallDocTrans_h

#include "LogTrans/LogTrans.h"
#include "SeLogUtil/LogOpr.h"
#include "TransUtil/CubicTrans.h"
#include "TransUtil/DocTrans.h"


class AosBatchAddSmallDocTrans : public AosDocTrans
{
private:
	u64 					mGroupId;
	u32 					mLogEntryNums;
	AosBuffPtr				mEntrysBuff;

public:
	AosBatchAddSmallDocTrans(const bool regflag);
	AosBatchAddSmallDocTrans(
			const u64 &docid,
			const u32 &logEntryNums,
			const AosBuffPtr &contents,
			const AosRundataPtr &rdata);
	~AosBatchAddSmallDocTrans();

	// Trans Interface	
	virtual bool serializeFrom(const AosBuffPtr &buff);
	virtual bool serializeTo(const AosBuffPtr &buff);
	virtual AosTransPtr clone();
	virtual bool proc();

};
#endif

