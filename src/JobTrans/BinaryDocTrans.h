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
// 05/17/2013	Created by Young Pan
////////////////////////////////////////////////////////////////////////////
#if 0
#ifndef Aos_JobTrans_BinaryDocTrans_h
#define Aos_JobTrans_BinaryDocTrans_h

#include "TransUtil/TaskTrans.h"
#include "API/AosApiR.h"
#include "TransUtil/Ptrs.h"
#include "TransUtil/AsyncReqTrans.h"
#include "Rundata/Rundata.h"
#include "SEInterfaces/DataScannerObj.h"

class AosBinaryDocTrans : public AosAsyncReqTrans
{

private:
	u64 mBinaryDocId;

public:
	AosBinaryDocTrans(const bool regflag);
	AosBinaryDocTrans(
			const u64 &buff,
			const int ser_id, 
			const bool need_save,
			const bool need_resp);
	~AosBinaryDocTrans();

	// Trans Interface	
	virtual bool serializeFrom(const AosBuffPtr &buff);
	virtual bool serializeTo(const AosBuffPtr &buff);
	virtual AosTransPtr clone();
	virtual bool proc();
};
#endif

#endif
