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
#ifndef Aos_DocTrans_ReadUnLockTrans_h
#define Aos_DocTrans_ReadUnLockTrans_h

#include "TransUtil/DocTrans.h"

class AosReadUnLockTrans : virtual public AosDocTrans
{

private:
	u64			mDocid;
	u64			mUserid;
	bool		mNeedBinaryData;

public:
	AosReadUnLockTrans(const bool regflag);
	AosReadUnLockTrans(
			const u64 docid,
			const u64 userid,
			const bool need_save,
			const bool need_resp,
			const u64 snap_id);

	~AosReadUnLockTrans();

	// Trans Interface	
	virtual bool serializeFrom(const AosBuffPtr &buff);
	virtual bool serializeTo(const AosBuffPtr &buff);
	virtual AosTransPtr clone();
	virtual bool proc();
	virtual bool procGetResp();

};
#endif

