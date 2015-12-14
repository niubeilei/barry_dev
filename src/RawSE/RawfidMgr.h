////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Zykie Networks, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// Description:
//	This class is used to manage raw files whose content are not the concern
//
// Modification History:
// 2014-11-21 created by White
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_RawSE_RawfidMgr_H_
#define Aos_RawSE_RawfidMgr_H_

#include "aosUtil/Types.h"
#include "ReliableFile/Ptrs.h"
#include "Rundata/Ptrs.h"
#include "Thread/Ptrs.h"
#include "Util/Ptrs.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"


class AosRawfidMgr : virtual public OmnRCObject
{
	OmnDefineRCObject;

private:
	enum
	{
		eDftBatchSize = 50	//TODO: get it from configuration
	};

	OmnMutexPtr			mLock;
	OmnMutex *			mLockRaw;
	OmnFilePtr			mFile;
	u64					mLastAllocatedID;	//last allocated local file id
	u64					mLastIDReadFromFile;
	int					mIdBatchSize;
	const char*			mHomeDir;

public:
	AosRawfidMgr();
	virtual ~AosRawfidMgr();
	u64	allocateRawfid(
			AosRundata *rdata,
			u32 partition_id);

private:
	bool readBatchFileIDs(AosRundata *rdata);
};


#endif /* RAWFILE_H_ */
