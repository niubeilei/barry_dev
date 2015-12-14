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
// 2013/12/23 Created by Ketty 
////////////////////////////////////////////////////////////////////////////
#if 0
#ifndef AOS_StreamEngine_DocidProvider_h
#define AOS_StreamEngine_DocidProvider_h

#include "Alarm/Alarm.h"
#include "Rundata/Ptrs.h"
#include "Thread/Mutex.h"
#include "SEInterfaces/TaskObj.h"
#include "SEInterfaces/TaskDataObj.h"
#include "SEInterfaces/TaskType.h"


#include "SEInterfaces/DataRecordObj.h"


class AosDocidProvider : public OmnRCObject 
{
	OmnDefineRCObject;

	enum
	{
		//eIncDocidsEach = 100,
		eIncDocidsEach = 1,
	};

	OmnString	mRcdTypeKey;
	u64			mRecordDocid;
	int			mDocSize;
	u64			mCrtSizeid;


	u64			mStartDocids;
	int			mRemainNum;

	OmnMutexPtr	mLock;

public:
	AosDocidProvider(	
		const AosRundataPtr &rdata,
		const AosTaskObjPtr	&task,
		const OmnString &rcd_type_key,
		const AosXmlTagPtr &record_doc,
		const int doc_size);
	
	~AosDocidProvider();

	bool 	getNextDocids(
				const AosRundataPtr &rdata,
				u64 &docid,
				int &num_docids);

private:
	bool 	init(const AosRundataPtr &rdata,
				const AosXmlTagPtr &record_doc);

	bool 	init(const AosRundataPtr &rdata,
				const AosXmlTagPtr &record_doc,
				const AosTaskDataObjPtr &task_data);

	bool 	getMoreDocids(
				const AosRundataPtr &rdata,
				u64 &docid,
				int &num_docids);

};

#endif

#endif
