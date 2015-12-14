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
#ifndef AOS_TaskMgr_DocidAllocator_h
#define AOS_TaskMgr_DocidAllocator_h

#include "Alarm/Alarm.h"
#include "Rundata/Ptrs.h"
#include "SEInterfaces/TaskObj.h"
#include "SEInterfaces/TaskDataObj.h"
#include "SEInterfaces/TaskType.h"


class AosDocidAllocator : public OmnRCObject 
{
	OmnDefineRCObject;

	enum
	{
		//eIncDocidsEach = 100,
		eIncDocidsEach = 40000,
	};

	AosTaskObjPtr mTask;
	OmnString	mRcdTypeKey;
	u64			mRecordDocid;
	int			mDocSize;
	u64			mCrtSizeid;

	u64			mStartDocids;
	int			mRemainNum;

public:
	AosDocidAllocator(
		const AosRundataPtr &rdata,
		const AosTaskObjPtr	&task,
		const OmnString &rcd_type_key,
		const AosXmlTagPtr &record_doc,
		const int doc_size);
	~AosDocidAllocator();

	bool 	getNextDocids(
				const AosRundataPtr &rdata,
				u64 &docid,
				int &num_docids);
	

private:
	bool 	init(const AosRundataPtr &rdata,
				const AosXmlTagPtr &record_doc,
				const AosTaskDataObjPtr &task_data);
	bool 	init(const AosRundataPtr &rdata,
				const AosXmlTagPtr &record_doc);

	bool 	getMoreDocids(
				const AosRundataPtr &rdata,
				u64 &docid,
				int &num_docids);

};

#endif
