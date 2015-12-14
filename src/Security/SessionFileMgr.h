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
// 10/02/2010	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_Security_SessionFileMgr_h
#define Aos_Security_SessionFileMgr_h

#include "aosUtil/Types.h"
#include "Thread/Ptrs.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"
#include "Util/Ptrs.h"


class AosSessionFileMgr : virtual public OmnRCObject
{
	OmnDefineRCObject;

public:
	enum
	{
		eMaxSeqno = 10,
		eMaxRecordId = 100000,
		eSeqnoOffset = 100,
		eRecordIdOffset = eSeqnoOffset + 4,

		eHeaderSize = 1000,
		eRecordSize = 50500,
	};

private:
	OmnMutexPtr	mLock;
	u32			mCrtSeqno;
	u32			mCrtRecordId;
	OmnString	mDirname;
	OmnString	mFname;
	OmnFilePtr	mFiles[eMaxSeqno];

public:
	AosSessionFileMgr();
	~AosSessionFileMgr();

	bool 	start();
	bool 	getLocation(u32 &seqno, u32 &record_id);
	OmnFilePtr	getFile(const u32 seqno);
	u32			getStart(const u32 recordid)
	{
		return eHeaderSize + (recordid * eRecordSize);
	}

private:
	bool 	setCrtSeqno();
	bool 	readCrtSeqno();
	bool 	openFile(const u32 seqno);
};
#endif

