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
#ifndef Aos_DocTrans_WriteLockTrans_h
#define Aos_DocTrans_WriteLockTrans_h

#include "TransUtil/DocTrans.h"

class AosWriteLockTrans : virtual public AosDocTrans
{
	enum
	{
		eError = 0,
		eWaitingForLock = 1
	};

private:
	u64			mDocid;
	u64			mUserid;
	int			mWaitTimer;
	int			mHoldTimer;
	int			mStatus;

public:
	AosWriteLockTrans(const bool regflag);
	AosWriteLockTrans(
			const u64 docid,
			const u64 userid,
			const int waittimer,
			const int holdtimer,
			const bool need_save,
			const bool need_resp,
			const u64 snap_id);
	~AosWriteLockTrans();

	// Trans Interface	
	virtual bool serializeFrom(const AosBuffPtr &buff);
	virtual bool serializeTo(const AosBuffPtr &buff);
	virtual AosTransPtr clone();
	virtual bool proc();
	virtual bool procGetResp();

	bool	writeDocObtained(const AosRundataPtr &rdata);

	
	bool	waitLockExpired(
				const OmnString &user_id,
				const OmnString &start_time,
				const OmnString &wait_time,
				const OmnString &lock_type);

};
#endif

