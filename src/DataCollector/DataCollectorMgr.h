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
// 12/29/2014 Created by Andy zhang
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_DataCollector_DataCollectorMgr_h
#define AOS_DataCollector_DataCollectorMgr_h

#include "API/AosApi.h"
#include "DataCollector/Ptrs.h"
#include "Rundata/Ptrs.h"
#include "SEInterfaces/LogicTaskObj.h"
#include "Thread/Mutex.h"
#include "Thread/ThrdShellProc.h"
#include "Thread/Ptrs.h"
#include "Util/Ptrs.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/BuffArray.h"
#include <list>
#include <set>

using namespace std;

class AosDataCollectorMgr : virtual public OmnRCObject
{
	OmnDefineRCObject;
private:
	typedef map<OmnString, AosLogicTaskObjPtr>::iterator mItr_t;

	map<OmnString, AosLogicTaskObjPtr>	mCollectors;

	OmnMutexPtr		mLock;
	u32				mFinishedNum;

public:
	AosDataCollectorMgr();
	~AosDataCollectorMgr();
	
	bool createDataCollector(
			const u64 &job_docid,
			const AosJobObjPtr &job,
			const AosXmlTagPtr &config, 
			OmnString &data_col_name,
			const AosRundataPtr &rdata);

	bool createDataCollectorLocked(
			const u64 &job_docid,
			const AosJobObjPtr &job,
			const AosXmlTagPtr &config, 
			OmnString &data_col_name,
			const AosRundataPtr &rdata);

	bool addOutput(
			const OmnString &collectorid, 
			const AosXmlTagPtr &config, 
			const AosRundataPtr &rdata);

	bool callBack(
			const OmnString &collectorid, 
			const AosXmlTagPtr &config, 
			const AosRundataPtr &rdata);

	bool callBackLocked(
			const OmnString &collectorid, 
			const AosXmlTagPtr &config, 
			const AosRundataPtr &rdata);

	bool getOutputFiles(
			const OmnString &collectorid,
			vector<AosXmlTagPtr> &files,
			const AosRundataPtr &rdata);

	bool getOutputFilesLocked(
			const OmnString &collectorid,
			vector<AosXmlTagPtr> &files,
			const AosRundataPtr &rdata);

	bool addTriggedTaskid(
			const OmnString &task_id, 
			const OmnString &collectorid, 
			const AosRundataPtr &rdata);

	bool finishDataCollector(
			const OmnString &collectorid, 
			const AosRundataPtr &rdata);

	bool checkAllFinished(const AosRundataPtr &rdata);

	AosLogicTaskObjPtr getLogicTask(
			const OmnString &collectorid, 
			const AosRundataPtr &rdata);

	AosLogicTaskObjPtr getLogicTaskLocked(
			const OmnString &collectorid, 
			const AosRundataPtr &rdata);

	bool cleanDataCol(
			const set<u64> &pdocids, 
			const set<u64> &cdocids, 
			const AosRundataPtr &rdata);
	bool cleanOutPut(
			const AosXmlTagPtr &task_doc,
			const AosRundataPtr &rdata);
	bool eraseActiveTask(
			const AosXmlTagPtr &task_doc, 
			const AosRundataPtr &rdata);

	void getAllDataCol(map<OmnString, AosLogicTaskObjPtr> &collectors);

	AosLogicTaskObjPtr getDataCol(
			const OmnString &collectorid, 
			const AosRundataPtr &rdata);

	AosLogicTaskObjPtr getDataColLocked(
			const OmnString &collectorid,
			const AosRundataPtr &rdata);

	bool reStartDataCol(
			const AosJobObjPtr &job,
			const OmnString &datacol_id,
			const AosRundataPtr &rdata);
private:
	void showDataCollectorStart(
			const u64 job_docid,
			const OmnString &logic_id);

};
#endif
