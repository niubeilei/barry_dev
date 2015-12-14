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
//	This class is used to manage a group of files, used to store 
//	either IILs or Documents. There are a number of files in the
//  group, each with a unique sequence number. Each file can 
//  store up to a given amount of data. 
//	
//	All documents are stored in 'mDocFilename' + seqno. There is 
//	a Document Index, that is stored in files 'mDocFilename' + 
//		'Idx_' + seqno
//
// Modification History:
// 12/11/2012	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_DocFileMgr_DfmTrans_h
#define AOS_DocFileMgr_DfmTrans_h

#include "Thread/ThrdShellProc.h"


class AosDfmTrans : virtual public OmnRCObject
{
	OmnDefineRCObject;

public:
	enum Status
	{
		eInvalid,

		eActive,
		eCommitting,
		eFinished,

		eMax
	};

private:
	u64				mTransId;
	OmnMutexPtr		mLock;
	bool			mReadFlag;
	Status			mStatus;
	bool			mFlush;
	bool			mCanBeCleaned;
	AosRundataPtr	mRundata;

public:
	AosDfmTrans(const u64 &transid);
	~AosDfmTrans();

	// OmnThrdShellProc interface
	virtual bool        run();
	virtual bool        procFinished();

	bool start();
	bool commit(const AosRundataPtr &rdata);
	bool rollback(const AosRundataPtr &rdata);
	bool isActive() const {return mStatus == eActive;}
	bool saveHeader(const u32 seqno,
				const u64 &offset, 
				const char *header, 
				const int len,
				const AosRundataPtr &rdata);
	bool readDoc(
				const u64 &docid,
				AosBuffPtr &buff, 
				const AosRundataPtr &rdata);

private:
	bool modifyHeaders(const AosRundataPtr &rdata);
	bool deleteOldBodies(const AosRundataPtr &rdata);
	bool cleanTrans(const AosRundataPtr &rdata);
	bool returnTrans(const AosRundataPtr &rdata);
};
#endif
