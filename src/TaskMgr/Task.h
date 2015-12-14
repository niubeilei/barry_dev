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
// 08/09/2011	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_TaskMgr_Task_h
#define AOS_TaskMgr_Task_h

#include "Alarm/Alarm.h"
#include "Job/Job.h"
#include "Rundata/Ptrs.h"
#include "SEInterfaces/JobObj.h"
#include "SEInterfaces/TaskObj.h"
#include "TaskMgr/Ptrs.h"
#include "TaskMgr/TaskThrd.h"
#include "TaskUtil/TaskStatus.h"
#include "Util/Ptrs.h"
#include "Util/String.h"
#include "XmlUtil/Ptrs.h"
#include "API/AosApi.h"
#include "SEInterfaces/TaskDataObj.h"
#include "SEInterfaces/TaskType.h"

#include<sstream>

class AosTask : public AosTaskObj, public AosTaskReporter
{
public:
	enum
	{
		eReStartNumTimes = 1
	};

	enum Status
	{
		eInvalid,

		eStop,
		eStart,
		eFail,
		ePaused,
		eAborted,
		eFinish,

		eMax
	};

	struct ActionInfo
	{
		u32                 mSeqno;
		AosActionObjPtr     mAction;
		AosXmlTagPtr        mSdoc;
		Status              mStatus;
		int                 mProgress;

		ActionInfo(const u32 seqno, const AosActionObjPtr &action, const AosXmlTagPtr &sdoc)
		:
		mSeqno(seqno),
		mAction(action),
		mSdoc(sdoc),
		mStatus(eStop),
		mProgress(0)
		{
		}

		ActionInfo(const ActionInfo &act_info)
		:
		mSeqno(act_info.mSeqno),
		mAction(act_info.mAction),
		mSdoc(act_info.mSdoc),
		mStatus(eStop),
		mProgress(0)
		{
		}

		ActionInfo()
		:
		mSeqno(0),
		mStatus(eStop),
		mProgress(0)
		{
		}
	};

private:
	OmnMutexPtr								mDocidLock;
	map<OmnString, AosDocidAllocatorPtr>	mDocidAllocators;

protected:
	OmnMutexPtr								mLock;
	OmnCondVarPtr							mCondVar;
	OmnThreadPtr							mStartTaskThread;
	AosTaskType::E      					mTaskType;
	OmnString           					mName;
	AosTaskStatus::E						mStatus;
	AosTaskErrorType::E						mErrorType;
	int										mPhysicalId;
	int										mProgress;
	bool									mJobPublic;
	bool									mIsDeleteFile;
	OmnString								mTaskId;
	OmnString								mLogicId;
	u64										mTaskDocid;
	u64										mJobDocid;
	OmnString								mTaskContainer;
	OmnString								mLogContainer;
	OmnString								mStartedTime;
	OmnString								mFinishedTime;
	u64										mStartStampTime;

	multimap<OmnString, AosTaskDataObjPtr>	mTaskOutPutMap;

	map<OmnString, OmnString>				mTaskENV;
	AosTaskDataObjPtr						mTaskData;
	int										mVersion;
	int										mMaxNumThread;
	bool									mTaskDataFinished;
	OmnString								mErrorMsg;
	AosRundataPtr							mRundata;

	bool									mTaskRunFinished;
	//map<OmnString, AosTaskDataObjPtr>		mSnapShots;

	//map<OmnString, AosTaskDataObjPtr> 		mJobSnapShots;
	map<u32, AosTaskDataObjPtr>				mIILSnapShots;

	int								mAveProcessTime;
	int 							mStartedNum;
	int 							mFinishedNum;
	int 							mFailedNum;

	static	AosTaskObjPtr			mTask;

	map<OmnString, i64>				mCounterMap;

public:
	AosTask(
		const AosTaskType::E type, 
		const OmnString &name, 
		const bool flag);
	~AosTask();

	virtual bool signal(const int threadLogicId);
	virtual bool startTask(const AosRundataPtr &rdata);
	virtual bool taskStarted(const AosRundataPtr &rdata);
	virtual bool taskFinished(const AosRundataPtr &rdata);
	virtual bool taskFailed(const AosRundataPtr &rdata);

	virtual bool isStoped() const {return mStatus == AosTaskStatus::eStop;}
	virtual bool isFinished() const {return mStatus == AosTaskStatus::eFinish;}
	virtual bool isStarted() const {return mStatus == AosTaskStatus::eStart;}
	virtual bool isPaused() const {return mStatus == AosTaskStatus::ePause;}
	virtual bool isFailed() const {return mStatus == AosTaskStatus::eFail;}

	virtual AosTaskType::E  getTaskType() const {return mTaskType;}

	virtual AosTaskStatus::E getStatus() const {return mStatus;}
	virtual void setStatus(const AosTaskStatus::E status) 
	{ 
		mStatus = status; 
		if (isFinished()) setProgress(100);
	}

	virtual int getPhysicalId() const {return mPhysicalId;}
	virtual void setPhysicalId(const int physicalid) {mPhysicalId = physicalid;}

	virtual int getProgress() const {return mProgress;}
	virtual void setProgress(const int progress) {mProgress = progress;}

	virtual OmnString getTaskId() const {return mTaskId;}
	virtual void setTaskId(const OmnString &taskid, const AosRundataPtr &rdata);

	virtual OmnString getLogicId() const {return mLogicId;}
	virtual void setLogicId(const OmnString &logicid) {mLogicId = logicid;}

	virtual u64 getTaskDocid() const {return mTaskDocid;}
	virtual void setTaskDocid(const u64 &taskdocid) {mTaskDocid = taskdocid;}

	virtual u64 getJobDocid() const {return mJobDocid;}

	virtual AosTaskDataObjPtr getTaskData() const {return mTaskData;}
	virtual void setTaskData(const AosTaskDataObjPtr &taskdata) {mTaskData = taskdata;}

	virtual AosRundataPtr getRundata() const { return mRundata; }
	virtual void setRundata(const AosRundataPtr &rdata) 
	{ 
		aos_assert(rdata);
		mRundata = rdata->clone(AosMemoryCheckerArgsBegin); 
	}

	virtual int	getVersion() const { return mVersion; }

	virtual bool getIsDeleteFile() const { return mIsDeleteFile; }

	virtual u64 getStartStampTime() const {return mStartStampTime;}

	void			setTaskENV(	map<OmnString, OmnString> &env) {mTaskENV = env;}
	void  			setTaskENV(const OmnString &key, const OmnString &value) {mTaskENV[key] = value;}
	OmnString		getTaskENV(
						const OmnString &key,
						const AosRundataPtr &rdata);

	virtual bool	logTask(
						const OmnString &logkey,
						const AosRundataPtr &rdata) const;
	virtual bool	updateTaskStatus(const AosRundataPtr &rdata);
	virtual bool	updateTaskProgress(const AosRundataPtr &rdata);
	virtual bool	updateTaskOutPut(
						const AosTaskDataObjPtr &task_data,
						const AosRundataPtr &rdata);

	virtual bool 	updateTaskSnapShots(
						vector<AosTaskDataObjPtr> &snapshots,
						const AosRundataPtr &rdata);

	virtual bool	actionIsFinished(const ActionInfo &action);
	virtual AosTaskDataObjPtr getOutPut(
						const AosTaskDataType::E &type,
						const OmnString &key,
						const AosRundataPtr &rdata);
	//virtual AosTaskDataObjPtr getSnapShot(
	//					const OmnString &key,
	//					const AosRundataPtr &rdata);
	virtual bool	initOutPut(const AosRundataPtr &rdata);

	virtual int		getResource(const OmnString &res_name, const AosRundataPtr &rdata);

	virtual bool	setTaskDataFinished();
	virtual bool	taskDataIsFinished();

	virtual bool setErrorType(const AosTaskErrorType::E &type);
	virtual bool setErrorMsg(const OmnString &msg);
	
	//bool commitSnapshots(const AosRundataPtr &rdata);

	virtual bool getNextDocids(
					const AosRundataPtr &rdata,
					u64 &docid,
					int &num_docids,
					OmnString &rcd_type_key,
					int &doc_size,
					AosXmlTagPtr &record_doc);
	virtual bool getDataset(map<OmnString, AosDatasetObjPtr> &set_map){ return false; };
	virtual map<OmnString, AosDatasetObjPtr> getDatasets(){ map<OmnString, AosDatasetObjPtr> datasets; return datasets; }
	virtual map<int, OmnString> getIndexs(){ map<int, OmnString> indexs; return indexs; }
	virtual AosXmlTagPtr getDataColTags(){ return 0; };
	virtual bool actionRunFinished(
						const AosRundataPtr &rdata,
						const AosActionObjPtr &action);
	
	virtual bool isTaskRunFinished(){ return mTaskRunFinished;};

	//virtual void getJobSnapShots(map<OmnString, AosTaskDataObjPtr> &snap_shot_map) {snap_shot_map = mJobSnapShots;}
	virtual void getIILSnapShots(map<u32, AosTaskDataObjPtr> &snap_shot_map) {snap_shot_map = mIILSnapShots;}

	virtual AosTaskObjPtr	create();
	virtual AosTaskObjPtr	create(
								const AosXmlTagPtr &sdoc, 
								const AosJobInfo &jobinfo,
								const AosTaskDataObjPtr &task_data,
								const OmnString &logic_id,
								const AosRundataPtr &rdata);
	virtual bool			createSnapShot(const AosRundataPtr &rdata);
	virtual AosXmlTagPtr	getTaskDoc(const AosRundataPtr &rdata);
	virtual bool			startAction(const AosRundataPtr &rdata);
	virtual bool 			initMemberAction(const AosRundataPtr &rdata);
	bool inputDataProcessed(AosRundataPtr rdata);
	bool updateJobStat(AosRundataPtr rdata);
	virtual AosXmlTagPtr 	getDataColTag(
								const OmnString &datacol_id, 
								const AosRundataPtr &rdata);
	virtual bool			setDataColTags(
								const AosXmlTagPtr &def,
								const AosRundataPtr &rdata);

	virtual AosValueRslt getRunTimeValue(
								const OmnString &key,
								const AosRundataPtr &rdata);

	virtual bool			setRunTimeValue(
								const OmnString &key,
								const AosValueRslt &value,
								const AosRundataPtr &rdata);

	static AosTaskObjPtr getTaskStatic(const u64 task_docid,
								const AosRundataPtr &rdata);	
	static	void resetTask();
	virtual void report(
			const char *file,
			const int line,
			const OmnString &action, 
			const OmnString &msg);

	virtual void updateCounter(const OmnString &str,
								const i64 counter);
	virtual void report();

private:
	bool    taskFinishedPriv(const AosRundataPtr &rdata);

	AosDocidAllocatorPtr getDocidAllocator(
							const AosRundataPtr &rdata,
							OmnString &rcd_type_key,
							int &doc_size,
							AosXmlTagPtr &record_doc);
	bool 	addReq(
				const AosRundataPtr &rdata,
				const AosTransPtr &trans,
				AosBuffPtr &resp)
	{
		bool timeout = false;
		bool rslt = AosSendTrans(rdata, trans, timeout, resp);
		aos_assert_r(rslt, false);
		aos_assert_r(!timeout, false);
		aos_assert_r(resp && resp->dataLen() >= 0, false);
		return true;
	}

	void showTaskInfo(
			const char *file, 
			const int line,
			const OmnString &action);
};


class TaskLog
{
	public:
		static TaskLog smTaskLog;

	public:
		void getLogStr(
				const u64 task_id,  
				std::ostringstream &os);

		static void log(const std::ostringstream &data);
};

#endif
