////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
//
// Modification History:
// 05/21/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SEInterfaces_TaskObj_h
#define Aos_SEInterfaces_TaskObj_h

#include "Rundata/Ptrs.h"
#include "SEInterfaces/Ptrs.h"
#include "SEInterfaces/TaskType.h"
#include "SEInterfaces/TaskCreatorObj.h"
#include "SEInterfaces/TaskDataType.h"
#include "SEInterfaces/JobObj.h"
#include "TaskUtil/TaskStatus.h"
#include "Util/String.h"
#include "Util/Ptrs.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "XmlUtil/Ptrs.h"
#include "TaskMgr/TaskErrorType.h"

#include <map>
#include <set>
#include <vector>

using namespace std;

class AosTaskReporter
{
	protected:
		AosTaskReporter * mTargetReporter;

	public:
		AosTaskReporter()
		:
		mTargetReporter(0)
		{
		}

		virtual void report(
				const char *file,
				const int line,
				const OmnString &action, 
				const OmnString &msg)
		{
			if (mTargetReporter)
				mTargetReporter->report(file, line, action, msg);
		}


		virtual void updateCounter(
				const OmnString &str,
				const i64 counter)
		{
			if (mTargetReporter)
				mTargetReporter->updateCounter(str, counter);
		}

		void setTargetReporter(AosTaskReporter * reporter)
		{
			mTargetReporter = reporter;
		}
};

class AosTaskObj : virtual public OmnRCObject
{
	OmnDefineRCObject;

	static AosTaskCreatorObjPtr		smTaskCreator;

public:
	virtual bool			isStoped() const = 0;
	virtual bool			isFinished() const = 0;
	virtual bool			isStarted() const = 0;
	virtual bool			isPaused() const = 0;
	virtual bool			isFailed() const = 0;

	virtual bool			startTask(const AosRundataPtr &rdata) = 0;
	virtual bool			taskFinished(const AosRundataPtr &rdata) = 0;
	virtual bool			taskStarted(const AosRundataPtr &rdata) = 0;
	virtual bool			taskFailed(const AosRundataPtr &rdata) = 0;

	virtual AosTaskDataObjPtr getTaskData() const = 0;
	virtual void setTaskData(const AosTaskDataObjPtr &taskdata) = 0;

	virtual AosTaskType::E	getTaskType() const = 0;

	virtual OmnString		getTaskId() const = 0;
	virtual void			setTaskId(const OmnString &taskid, const AosRundataPtr &rdata) = 0;

	virtual int				getPhysicalId() const = 0;
	virtual void			setPhysicalId(const int physicalid) = 0;

	virtual u64				getTaskDocid() const = 0;
	virtual void			setTaskDocid(const u64 &taskdocid) = 0;

	virtual u64 			getJobDocid() const = 0;

	virtual AosTaskStatus::E getStatus() const = 0;
	virtual void			setStatus(const AosTaskStatus::E st) = 0;

	virtual AosRundataPtr	getRundata() const = 0;
	virtual void			setRundata(const AosRundataPtr &rdata) = 0;

	virtual OmnString		getLogicId() const = 0;
	virtual void			setLogicId(const OmnString &logic_id) = 0;

	virtual int 			getProgress() const = 0;
	virtual void			setProgress(const int p) = 0;

	virtual int				getVersion() const = 0;
	virtual u64 			getStartStampTime() const = 0;
	virtual bool			getIsDeleteFile() const = 0;

	virtual AosTaskObjPtr	create() = 0;
	virtual AosTaskObjPtr	create(
								const AosXmlTagPtr &sdoc, 
								const AosJobInfo &jobinfo,
								const AosTaskDataObjPtr &task_data,
								const OmnString &logic_id,
								const AosRundataPtr &rdata) = 0;
	
	virtual void			clear() = 0;
	virtual bool			checkConfig(
								const AosXmlTagPtr &def,
								const AosRundataPtr &rdata) = 0;

	virtual bool			actionProgressed(
								const int percent,
								const AosRundataPtr &rdata) = 0; 
	virtual bool			actionFinished(
								const AosActionObjPtr &action,
								const AosRundataPtr &rdata) = 0;
	virtual bool			actionFailed(
								const AosActionObjPtr &action,
								const AosRundataPtr &rdata) = 0;

	virtual bool			serializeFrom(
								const AosXmlTagPtr &doc,
								const AosRundataPtr &rdata) = 0;

	virtual bool			serializeTo(
								const AosXmlTagPtr &doc,
								const AosRundataPtr &rdata) = 0;

	virtual AosXmlTagPtr	getTaskDoc(const AosRundataPtr &rdata) = 0;

	virtual int 			getResource(const OmnString &res_name, const AosRundataPtr &rdata) = 0;
	virtual bool 			initMemberAction(const AosRundataPtr &rdata) = 0;

	virtual void 			onThreadInit(const AosRundataPtr &rdata, void **data) {};
	virtual bool			runAction(const AosRundataPtr &rdata , void *data=0) = 0;
	virtual void 			onThreadExit(const AosRundataPtr &rdata, void *data) {};

	virtual AosXmlTagPtr 	getDataColTag(
								const OmnString &datacol_id, 
								const AosRundataPtr &rdata) = 0;
	virtual bool			setDataColTags(
								const AosXmlTagPtr &def,
								const AosRundataPtr &rdata) = 0;
	virtual void			setTaskENV(map<OmnString, OmnString> &env) = 0;
	virtual OmnString		getTaskENV( 
								const OmnString &key,
								const AosRundataPtr &rdata) = 0;

	virtual	bool			updateTaskOutPut(
								const AosTaskDataObjPtr &task_data,
								const AosRundataPtr &rdata) = 0;

	virtual bool			updateTaskSnapShots(
								vector<AosTaskDataObjPtr> &snaphsots,
								const AosRundataPtr &rdata) = 0;

	virtual bool			initOutPut(const AosRundataPtr &rdata) = 0;

	virtual AosTaskDataObjPtr getOutPut(
								const AosTaskDataType::E &type,
								const OmnString &key,
								const AosRundataPtr &rdata) = 0;
	//virtual AosTaskDataObjPtr getSnapShot(
	//							const OmnString &key,
	//							const AosRundataPtr &rdata) = 0;
	virtual bool			setTaskDataFinished() = 0;
	virtual bool			taskDataIsFinished() = 0;
	virtual bool			allActionIsFinished(const AosRundataPtr &rdata) = 0;
	virtual bool			finishedAction(const AosRundataPtr &rdata) = 0;
	virtual bool			startAction(const AosRundataPtr &rdata) = 0;
	virtual bool			createSnapShot(const AosRundataPtr &rdata) = 0;
	virtual bool			checkTaskSuccessLock(bool &all_success) = 0;
	virtual bool 			setErrorType(const AosTaskErrorType::E &type) = 0;

	virtual bool		 getNextDocids(
							const AosRundataPtr &rdata,
							u64 &docid,
							int &num_docids,
							OmnString &rcd_type_key,
							int &doc_size,
							AosXmlTagPtr &record_doc) = 0;

	virtual bool getDataset(map<OmnString, AosDatasetObjPtr> &set_map) = 0;
	virtual map<OmnString, AosDatasetObjPtr> getDatasets() = 0;
	virtual map<int, OmnString> getIndexs() = 0;
	virtual bool		actionRunFinished(	
							const AosRundataPtr &rdata,
							const AosActionObjPtr &action) = 0;
	virtual bool 		isTaskRunFinished() = 0;
	virtual AosXmlTagPtr getDataColTags() = 0;

	static void setCreator(const AosTaskCreatorObjPtr &d);
	static AosTaskCreatorObjPtr getCreator() {return smTaskCreator;}

	static bool				checkConfigStatic(
								const AosXmlTagPtr &def,
								map<OmnString, OmnString> &jobenv,
								const AosRundataPtr &rdata);

	static AosTaskObjPtr	createTask(
								const AosXmlTagPtr &tag, 
								const AosJobInfo &jobinfo,
								const AosTaskDataObjPtr &task_data,
								const OmnString &logic_id,
								const AosRundataPtr &rdata);
	
	static AosTaskObjPtr	serializeFromStatic(
								const AosXmlTagPtr &doc,
								const AosRundataPtr &rdata);

	//virtual void getJobSnapShots(map<OmnString, AosTaskDataObjPtr> &snap_shot_map) = 0;

	virtual AosValueRslt 	getRunTimeValue(
								const OmnString &key,
								const AosRundataPtr &rdata) = 0;
	virtual bool			setRunTimeValue(
								const OmnString &key,
								const AosValueRslt &value,
								const AosRundataPtr &rdata) = 0;
	virtual void getIILSnapShots(map<u32, AosTaskDataObjPtr> &snap_shot_map) = 0;
	virtual void report() = 0;
protected:
	virtual bool			registerTask(
								const AosTaskType::E type,
								const OmnString &name,
								const AosTaskObjPtr &task);
};
#endif
