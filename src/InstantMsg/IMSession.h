////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// Modification History:
// 10/25/2011	Created by Joshi Peng
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_IMSession_IMSession_h
#define AOS_IMSession_IMSession_h

#include "Rundata/Rundata.h"
#include "Thread/Mutex.h"
#include "Util/RCObject.h"

class AosIMSession : virtual public OmnRCObject
{
	OmnDefineRCObject;

private:
	OmnMutexPtr			mLock;
	OmnString    		mMsgs;
	OmnString			mSenderCid;
	OmnString			mRecverCid;

public:
	AosIMSession(
			const OmnString &sender_cid,
			const OmnString &recver_cid);

	~AosIMSession();

	bool addMsg(
			const OmnString &msg, 
			const AosRundataPtr &rdata);

	bool finishSession(
			const OmnString &recver_cid,
			const AosXmlTagPtr &notes,
			const AosRundataPtr &rdata);
};
#endif
