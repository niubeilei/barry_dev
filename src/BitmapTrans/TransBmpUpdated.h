////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//	
// Description:
//
// Modification History:
// 2013/04/30 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_BitmapTrans_TransBmpUpdated_h
#define Aos_BitmapTrans_TransBmpUpdated_h

#include "BitmapTrans/Ptrs.h"
#include "Query/Ptrs.h"
#include "Rundata/Ptrs.h"
#include "TransUtil/BitmapTrans.h"
#include "Thread/ThrdShellProc.h"
#include "Util/Ptrs.h"
using namespace std;

class AosTransBmpUpdated : public AosBitmapTrans, 
						   public OmnThrdShellProc
{
	OmnDefineRCObject;

private:
	u64			mExecutorID;
	int			mSendCubeId;
	OmnString	mErrmsg;

public:
	AosTransBmpUpdated(const bool regflag);
	AosTransBmpUpdated(
			const AosRundataPtr &rdata, 
			const u64 executor_id,
			const int recv_phy_id, 
			const int send_phy_id, 
			const OmnString &errmsg);
	~AosTransBmpUpdated();

	// OmnThrdShellProc interface
	virtual bool		run();
	virtual bool		procFinished();

	virtual bool serializeFrom(const AosBuffPtr &buff);
	virtual bool serializeTo(const AosBuffPtr &buff);
	virtual AosTransPtr clone();
	virtual bool proc();
};
#endif
