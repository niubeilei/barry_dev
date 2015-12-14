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
#ifndef AOS_DataCollector_DataCollector_h
#define AOS_DataCollector_DataCollector_h

#include "API/AosApi.h"
#include "DataCollector/Ptrs.h"
#include "DataCollector/DataCollectorType.h"
#include "Rundata/Ptrs.h"

#include "TaskUtil/LogicTask.h"
#include "Job/JobMgr.h"
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

class AosDataCollector : public AosLogicTask
{

protected:
	enum E
	{
		eInvalid, 
		
		eSort,
		eGroup,
		eNorm,
		eBuff,

		eMax
	};

protected:
	E								mType;
	OmnString						mDataColId;
	OmnString						mParentDataColId;
	AosXmlTagPtr					mDataColTag;
	AosXmlTagPtr					mDataProcConf;

	u32								mStartNum;  // N map tasks used this the datacollector;
	u16								mPhyNum;
	u16								mGroupNum;
	bool							mFinished;
	int								mShuffleId;
	bool							mIsInited;
	//set<u64>						mActiveTaskDocids;
	vector< list<AosXmlTagPtr> >	mFileList;
	vector<OmnString>				mTaskIds;
	vector<AosXmlTagPtr>			mLastOutputFiles;

public:
	AosDataCollector(const u64 &job_docid);
	virtual ~AosDataCollector();
	
	virtual bool addOutput(const AosXmlTagPtr &output, const AosRundataPtr &rdata);
	virtual bool getOutputFiles(vector<AosXmlTagPtr> &files, const AosRundataPtr &rdata);
	virtual bool addTriggedTaskid(const OmnString &task_id, const AosRundataPtr &rdata);
	virtual bool config(const AosJobObjPtr &job,const AosXmlTagPtr &conf, const AosRundataPtr &rdata);
	virtual bool finishDataCollector(const AosRundataPtr &rdata);
	virtual bool callBack(const AosXmlTagPtr &output, const AosRundataPtr &rdata);
	virtual bool isFinished() const ;
	virtual bool isInited() const ;
	virtual AosXmlTagPtr	getTaskDoc(const AosRundataPtr &rdata);

	virtual void setDataColId(const OmnString &name) {mDataColId = name;}
	virtual OmnString getDataColId() { return mDataColId;}
	virtual void setParentDataColId(const OmnString &name, const int &shuff_id)
	{ 
		mParentDataColId = name;
		mShuffleId = shuff_id;
	}

	virtual void addStartNum(){mStartNum++;};

	virtual bool taskFinished(const AosJobObjPtr &job, const AosTaskRunnerInfoPtr &run_info, const AosRundataPtr &rdata);
	virtual bool taskInfoFinished(const u64 &task_docid, const AosRundataPtr &rdata);

	virtual bool cleanOutPut(const int level, const u64 &task_docid, const AosRundataPtr &rdata);
	virtual bool merge(const AosLogicTaskObjPtr &datacol);
	virtual AosCompareFunPtr getComp();
	virtual E getDataColType() {return mType; };
	static bool isValid (const E code) {return code > eInvalid && code < eMax;}
	static AosLogicTaskObjPtr createDataCol(const u64 &job_docid, const AosXmlTagPtr &conf, const AosRundataPtr &rdata);

};
#endif
