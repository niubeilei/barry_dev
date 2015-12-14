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
//
// Modification History:
// 2012/04/04 Created by Ken Lee
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_StorageEngine_Testers_DocInfo3_h
#define Aos_StorageEngine_Testers_DocInfo3_h

#include "alarm_c/alarm.h"
#include "Rundata/Ptrs.h"
#include "SEInterfaces/TaskObj.h"
#include "Thread/Ptrs.h"
#include "Util/String.h"
#include "Util/Ptrs.h"
#include "Util/OmnNew.h"
#include "XmlUtil/Ptrs.h"

class AosDocInfo3 : public OmnRCObject
{
	OmnDefineRCObject;
	
	enum
	{
		eDftFirstDocOffset = 1000
	};

	OmnMutexPtr		mLock;
	char *			mRawData;
	u64 *			mDocids;
	int				mDocSize;
	u32				mNumDocs;
	u32				mMaxDocs;
	u32				mServerId;
	u64				mLastSizeid;
	u64				mTotalNumDocs;
	u64				mJobServerId;
	u64				mJobDocid;

public:
	AosDocInfo3(const u32 serverid, const u64 &job_docid, const u64 &job_serverid);
	~AosDocInfo3();

	bool			createMemory(
						const u64 &buff_len,
						const int doc_size);

	bool			addDoc(
						const char *data, 
						const int len, 
						const u64 &docid,
						const AosRundataPtr &rdata);

	bool			sendStart(const AosRundataPtr &rdata);
	bool			sendFinish(const AosRundataPtr &rdata);

	u32 			getMaxDocs(){return mMaxDocs;}

	u64				getTotalNumDocs() {return mTotalNumDocs;}
private:
	bool			sendPriv(const AosRundataPtr &rdata);
};
#endif

