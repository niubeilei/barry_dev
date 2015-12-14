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
#ifndef Aos_DocTrans_BatchSendDocidsTrans_h
#define Aos_DocTrans_BatchSendDocidsTrans_h

#include "DocServer/DocBatchReaderMgr.h"
#include "DocServer/DocBatchReaderReq.h"
#include "TransUtil/TaskTrans.h"

class AosBatchSendDocidsTrans : virtual public AosTaskTrans 
{

private:
	OmnString					mScannerId;
	AosDocBatchReaderReq::E		mReqType;
	AosBuffPtr					mCont;
	
public:
	AosBatchSendDocidsTrans(const bool regflag);
	AosBatchSendDocidsTrans(
			const OmnString &scanner_id,
			const AosDocBatchReaderReq::E type,
			const AosBuffPtr &cont,
			const int svr_id);
	~AosBatchSendDocidsTrans();

	// Trans Interface	
	virtual bool serializeFrom(const AosBuffPtr &buff);
	virtual bool serializeTo(const AosBuffPtr &buff);
	virtual AosTransPtr clone();
	virtual bool proc();

};
#endif
#endif

