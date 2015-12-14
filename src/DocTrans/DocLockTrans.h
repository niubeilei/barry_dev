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
#ifndef Aos_DocTrans_DocLockTrans_h
#define Aos_DocTrans_DocLockTrans_h

#include "TransUtil/DocTrans.h"

class AosDocLockTrans : virtual public AosDocTrans
{

private:
	u64			mDocid;
	OmnString	mLockType;
	u64			mLockTimer;
	u64			mLockid;

public:
	AosDocLockTrans(const bool regflag);
	AosDocLockTrans(
			const u64 docid,
			const OmnString &lock_type,
			const u64 lock_timer,
			const u64 lockid,
			const bool need_save,
			const bool need_resp,
			const u64 snap_id);
	~AosDocLockTrans();

	// Trans Interface	
	virtual bool serializeFrom(const AosBuffPtr &buff);
	virtual bool serializeTo(const AosBuffPtr &buff);
	virtual AosTransPtr clone();
	virtual bool proc();

};
#endif

