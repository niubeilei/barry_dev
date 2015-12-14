////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// This type of IIL maintains a list of (string, docid) and is sorted
// based on the string value. 
//
// Modification History:
// 04/09/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_DataCollector_DataCollectorGroup_h
#define AOS_DataCollector_DataCollectorGroup_h

#include "API/AosApi.h"
#include "DataCollector/DataCollector.h"
#include "DataCollector/DataCollectorType.h"
#include "Rundata/Ptrs.h"
#include "TaskUtil/LogicTask.h"
#include "Thread/Mutex.h"
#include "Thread/ThrdShellProc.h"
#include "Thread/Ptrs.h"
#include "Util/Ptrs.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/BuffArray.h"

#include <list>
#include <vector>

using namespace std;

class AosDataCollectorGroup : public AosDataCollector 
{

public:
	enum E
	{
		eInvalid,

		eCubeShuffler,
		ePhyShuffler,

		eMax
	}; 

public:
	//OmnMutexPtr						mLock;
	AosDataCollectorGroup::E 		mGroupType;
	vector<AosLogicTaskObjPtr>		mChilds;
	u32								mChildSize;
	u32             				mFinishedNum;

public:
	AosDataCollectorGroup(const u64 &job_docid);
	virtual ~AosDataCollectorGroup();
	
	virtual bool addOutput(const AosXmlTagPtr &output, const AosRundataPtr &rdata);
	virtual bool getOutputFiles(vector<AosXmlTagPtr> &files, const AosRundataPtr &rdata);
	virtual bool addTriggedTaskid(const OmnString &task_id, const AosRundataPtr &rdata);
	virtual bool config(const AosJobObjPtr &job, const AosXmlTagPtr &conf, const AosRundataPtr &rdata);
	virtual bool finishDataCollector(const AosRundataPtr &rdata);
	virtual bool callBack(const AosXmlTagPtr &output, const AosRundataPtr &rdata);
	virtual bool isFinished();

	virtual OmnString getDataColId() { return mDataColId;}
	virtual int getShuffleType() {return (int)mGroupType; }

private:
	void toEnum(const OmnString &group_type);
	bool isValid(const E code) {return code > eInvalid && code < eMax;}
	bool dataCollectorFinished(const AosRundataPtr &rdata);
	bool mergePre(const AosLogicTaskObjPtr &datacol);
	void showCreateDataCollector(const OmnString &col_id);
	void showDataCollectorFinish();

};
#endif
