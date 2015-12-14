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
// A job creates one or more tasks. When all tasks finishes, the job
// finishes.
//
// Modification History:
// 12/29/2014 Modify by Jozhi
////////////////////////////////////////////////////////////////////////////
#include "Job/Job.h"

#include "EventMgr/Event.h"
#include "EventMgr/EventTriggers.h"
#include "IILEntryMap/IILEntryMapMgr.h"
#include "Job/JobMgr.h"
#include "Job/JobLogKeys.h"
#include "Job/JobNames.h"
#include "Job/JobSplitter.h"
#include "TaskUtil/Recycle.h"
#include "TaskScheduler/TaskScheduler.h"
#include "TaskScheduler/Ptrs.h"
#include "NetworkMgrUtil/CommandRespCode.h"
#include "QueryClient/QueryClient.h"
#include "Rundata/Rundata.h"
#include "SEInterfaces/TaskObj.h"
#include "SEInterfaces/TaskDataObj.h"
#include "SEInterfaces/BitmapObj.h"
#include "SEInterfaces/SeLogClientObj.h"
#include "SEInterfaces/QueryContextObj.h"
#include "SEInterfaces/QueryRsltObj.h"
#include "SEInterfaces/QueryReqObj.h"
#include "TaskUtil/TaskDriver.h"
#include "TaskUtil/TaskUtil.h"
#include "SEUtil/SysLogName.h"
#include "SEModules/SnapshotIdMgr.h"
#include "Thread/ThreadPool.h"
#include "UtilTime/TimeUtil.h"
#include "XmlUtil/DocTypes.h"
#include "TaskMgr/TaskData.h"
#include "TaskUtil/MapTaskCreator.h"
#include "TaskUtil/LogicTaskVer1.h"
#include "JobTrans/SendClearIILEntryMapTrans.h"
#include "JobTrans/ClearCubeIILEntryMapTrans.h"
#include "Debug/Debug.h"
#include "Util/File.h"
#include "Util/ReadFile.h"
#include "Util/DiskStat.h"
#include "SEInterfaces/TaskType.h"

static OmnMutex		sgLock;
static AosJobObjPtr	sgJobs[AosJobId::eMax];
static bool			sgInited = false;
static OmnThreadPoolPtr sgThreadPool = OmnNew OmnThreadPool("job", __FILE__, __LINE__);
JobLog  JobLog::smJobLog;

void
JobLog::getLogStr(
		const u64 job_id, 
		std::ostringstream &os,
		const OmnString &file,
		const OmnString &line)
{
	struct  timeval    tv;     
	struct  timezone   tz; 
	gettimeofday(&tv,&tz);      

	time_t time = tv.tv_sec;                                           
	struct tm result;                                                    
	localtime_r(&time, &result);
	char time_str[25];
	sprintf(time_str, "%02d%02d-%02d:%02d:%02d.%06ld", result.tm_mon+1, result.tm_mday, result.tm_hour, result.tm_min, result.tm_sec, tv.tv_usec);
	os << time_str << " JobId:" << job_id << " <"
		<< file << ":" << line << "> ";
}

void
JobLog::log(const std::ostringstream &data)
{
	OmnScreen << data.str() << endl;
}


AosJob::AosJob(
		const AosJobId::E type, 
		const OmnString &name, 
		const bool regflag)
:
mLock(OmnNew OmnMutex()),
mType(type),
mStatus(AosJobStatus::eStop),
mIsPublic(false),
mIsDeleteFile(false),
mSingleRun(false),
mJobServerId(AosGetSelfServerId()),
mJobProgress(0),
mJobDocid(0),
mThrdShellError(false),
mVersion(0)
{
	if (regflag) registerJob(this, type, name);
	OmnScreen << "new job" << endl;
}


AosJob::~AosJob()
{
	clear();
	OmnScreen << "delete job" << endl;
}


bool
AosJob::staticInit()
{
	if (sgInited)	return true;
	static AosJob	rootJob(AosJobId::eRootJob, AOSJOBNAME_ROOTJOB, true);
	sgInited = true;
	return true;
}

	
bool
AosJob::registerJob(
		const AosJob *job,
		const AosJobId::E type,
		const OmnString &name)
{
	if (!AosJobId::isValid(type))
	{
		OmnAlarm << "Invalid job id: " << type << enderr;
		return false;
	}

	sgLock.lock();
	if (sgJobs[type])
	{
		sgLock.unlock();
		OmnAlarm << "Job already registered: " << name << ":" << type << enderr;
		return false;
	}

	sgJobs[type] = (AosJob *)job;
	bool rslt = AosJobId::addName(name, type);
	sgLock.unlock();
	if (!rslt)
	{
		OmnAlarm << "Failed add job name" << enderr;
	}
	return rslt;
}


AosJobObjPtr
AosJob::staticCreateJob(
		const AosJobMgrObj::OprId id,
		const AosXmlTagPtr &jobdoc,
		const AosXmlTagPtr &sdoc, 
		const AosRundataPtr &rdata)
{
	if (!sgInited) staticInit();
	aos_assert_r(jobdoc && sdoc, 0);

	bool rslt = AosCheckSecurityCreateJob(jobdoc, rdata);
	if (!rslt) return 0;

	AosJobId::E jobid = AosJobId::toEnum(jobdoc->getAttrStr(AOSTAG_JOBID));
	if (!AosJobId::isValid(jobid))
	{
		AosSetErrorU(rdata, "invalid_job_id:") << jobdoc->toString();
		OmnAlarm << rdata->getErrmsg() << enderr;
		return 0;
	}

	AosJobObjPtr job = sgJobs[jobid];
	if (!job)
	{
		AosSetErrorU(rdata, "job_not_defined:") << jobid;
		OmnAlarm << rdata->getErrmsg() << enderr;
		return 0;
	}

	return job->create(id, jobdoc, sdoc, rdata);
}

bool
AosJob::addErrorMsg(
		const AosXmlTagPtr &job_doc,
		const OmnString &error_msg,
		const AosRundataPtr &rdata)
{
	aos_assert_r(job_doc, false);
	job_doc->setAttr(AOSTAG_STATUS, AosJobStatus::toStr(AosJobStatus::eFail));
	AosXmlTagPtr error_msgs = job_doc->getFirstChild("error_msgs");
	if (!error_msgs)
	{
		error_msgs = job_doc->addNode1("error_msgs");
	}
	aos_assert_r(error_msgs, false);
	OmnString str = "<error_msg>";
	str << error_msg << "</error_msg>";
	AosXmlParser parser;
	AosXmlTagPtr error = parser.parse(str, "" AosMemoryCheckerArgs);
	aos_assert_r(error, false);
	error_msgs->addNode(error);
	bool rslt = AosModifyDoc(job_doc, rdata);
	aos_assert_r(rslt, false);
	return true;
}

void 
AosJob::setRundata(const AosRundataPtr &rdata) 
{
	aos_assert(rdata);
	mRundata = rdata->clone(AosMemoryCheckerArgsBegin);
}

map<OmnString, OmnString>& 
AosJob::getJobENV() 
{
	return mJobENV;
}


OmnString 
AosJob::getTaskContainer() const
{
	return mJobContainerObjid;
}

OmnString 
AosJob::getLogContainer() const
{
	return mLogContainerObjid;
}


AosXmlTagPtr 
AosJob::getJobDoc() const 
{
	return mJobDoc;
}

AosRundataPtr 
AosJob::getRundata() const 
{
	return mRundata;
}

	
int
AosJob::getVersion()
{ 
	return mVersion; 
}

int 
AosJob::getJobPhysicalId() const 
{
	return mJobServerId;
}


bool 
AosJob::isJobPublic() const 
{
	return mIsPublic;
}

bool 
AosJob::getSingleRunningStatus() const 
{
	return mSingleRun;
}

bool 
AosJob::isStoped() const 
{
	return mStatus == AosJobStatus::eStop;
}

bool 
AosJob::isStarted() const 
{
	return mStatus == AosJobStatus::eStart;
}

bool 
AosJob::isFinished() const 
{
	return mStatus == AosJobStatus::eFinish;
}

bool 
AosJob::isWaited() const 
{
	return mStatus == AosJobStatus::eWait;
}

bool 
AosJob::isFailed() const 
{
	return mStatus == AosJobStatus::eFail;
}

bool 
AosJob::getIsDeleteFile() const 
{ 
	return mIsDeleteFile; 
}

AosDataCollectorMgrPtr 
AosJob::getDataColMgr() 
{ 
	return mDataColMgr;
}

AosJobObjPtr 
AosJob::create(
		const AosJobMgrObj::OprId id,
		const AosXmlTagPtr &jobdoc, 
		const AosXmlTagPtr &sdoc, 
		const AosRundataPtr &rdata) const
{
	AosJobObjPtr job = OmnNew AosJob(AosJobId::eRootJob, AOSJOBNAME_ROOTJOB, false);

	bool rslt;
	int version = sdoc->getAttrInt("zky_job_version", 0);
	if(version == 1)
	{
		rslt = job->initJob(id, jobdoc, sdoc, rdata);	
	}
	if (!rslt)
	{
		AosSetEntityError(rdata, "job_initjob_failed",
			"job", "initjobfailed");
		job->clear();
		bool rslt = job->addErrorMsg(job->getJobDoc(), rdata->getErrmsg(), rdata);
		aos_assert_r(rslt, 0);
		return 0;
	}
	return job;
}

bool
AosJob::createJobENV(
		const AosXmlTagPtr &doc,
		const AosRundataPtr &rdata)
{
	aos_assert_r(doc,false);

	AosXmlTagPtr entry_tag;
	AosXmlTagPtr env_tag = doc->getFirstChild(AOSTAG_ENV);
	if (!env_tag || !(entry_tag = env_tag->getFirstChild(true)))
	{
		return true;
	}
		
	OmnString env_key, env_value;
	while (entry_tag)
	{
		env_key = entry_tag->getAttrStr(AOSTAG_KEY);
		if (env_key != "")
		{
			env_value = entry_tag->getNodeText();
			if (env_value == "")
			{
				AosSetErrorU(rdata, "missing_env_value:") << entry_tag->toString();
				OmnAlarm << rdata->getErrmsg() << enderr;
				return false;
			}
			mJobENV[env_key] = env_value;	
		}
		entry_tag = env_tag->getNextChild();	
	}

	return true;
}


bool
AosJob::integrateResources(const AosRundataPtr &rdata)
{
	map<OmnString, AosXmlTagPtr>::iterator output_itr;
	map<OmnString, AosXmlTagPtr>::iterator reduce_itr = mReduceResources.begin();
	while(reduce_itr != mReduceResources.end())
	{
		OmnString r_key = reduce_itr->first;
		output_itr = mOutPutResources.find(r_key);
		aos_assert_r(output_itr != mOutPutResources.end(), false);
		if (output_itr != mOutPutResources.end())
		{
OmnScreen << "integrateResource erase: " << (output_itr->second)->toString() << endl;
			mOutPutResources.erase(output_itr);
		}
		reduce_itr++;
	}

	OmnString datacol_id;
	AosXmlTagPtr tag;
	AosTaskDataType::E type;
	output_itr = mOutPutResources.begin();
	while (output_itr != mOutPutResources.end())
	{
		tag = output_itr->second;
		type = AosTaskDataType::toEnum(tag->getAttrStr(AOSTAG_TYPE));
		if (type == AosTaskDataType::eOutPutIIL)
		{
			datacol_id = tag->getAttrStr(AOSTAG_DATACOLLECTOR_ID);
			aos_assert_r(datacol_id != "", false);
OmnScreen << "integrateResource addOutput: " << tag->toString() << endl;
			bool rslt = mDataColMgr->addOutput(datacol_id, tag, rdata);
			aos_assert_r(rslt, false);
		}
		output_itr++;
	}
	mReduceResources.clear();
	mOutPutResources.clear();

	return true;
}

bool
AosJob::finishDataCollector(const AosRundataPtr &rdata)
{
	map<OmnString, AosLogicTaskObjPtr>::iterator itr = mLogicTasks.begin();
	while(itr != mLogicTasks.end())
	{
		if (itr->second->isFinished())
		{
			set<OmnString> col_ids = itr->second->getDataColIds();
			set<OmnString>::iterator d_itr = col_ids.begin();
			while(d_itr != col_ids.end())
			{
OmnScreen << "jozhi finishDataCollector: " << *d_itr << endl;
				bool rslt = mDataColMgr->finishDataCollector(*d_itr, rdata);
				aos_assert_r(rslt, false);
				d_itr++;
			}
		}
		itr++;
	}
	return true;
}


bool 
AosJob::startJob(
		const AosJobMgrObj::OprId opr_id,
		const AosRundataPtr &rdata)
{
	AOSLOCK(mLock);

	bool rslt = clearIILEntryMap(rdata);
	aos_assert_r(rslt, false);

	rslt = setStatus(AosJobStatus::eStart, rdata);
	aos_assert_rl(rslt, mLock, false);

	aos_assert_rl(mJobDoc, mLock, false);
	aos_assert_rl(mLogicTasks.size() != 0, mLock, false);
	
	mJobDoc->setAttr(AOSTAG_JOBSERVERID, mJobServerId);
	AosModifyDoc(mJobDoc, rdata);

	mStartTime = mJobDoc->getAttrStr(AOSTAG_JOB_STARTED_TIME);
	if (mStartTime == "")
	{
		mStartTime = OmnGetTime(AosLocale::eChina);
		mJobDoc->setAttr(AOSTAG_JOB_STARTED_TIME, mStartTime);
		AosModifyDoc(mJobDoc, rdata);
	}
	OmnScreen << "Job Started: " << mStartTime << endl;

	showJobInfo("start");
	AOSUNLOCK(mLock);

	switch(opr_id)
	{
	case AosJobMgrObj::eReStart:
	case AosJobMgrObj::eSwitchJobMaster:
			rslt = recoverAllTask(opr_id, rdata);
			aos_assert_r(rslt, false);

			rslt = integrateResources(rdata);
			aos_assert_r(rslt, false);

			rslt = finishDataCollector(rdata);
			break;

	case AosJobMgrObj::eNormal:
			rslt = startNextLogicTask(rdata, opr_id);
			aos_assert_r(rslt, false);
			break;

	default:
			OmnAlarm << "not handle this opr_id" << enderr;
			return false;
	}

	AOSLOCK(mLock);
	rslt = scheduleTasks(rdata);

	if (!rslt)
	{
		logJob(AOSJOBLOG_JOB_FAILED_START_TASK, rdata);
		jobFailedLocked(rdata);
		AOSUNLOCK(mLock);

		AosSetErrorU(rdata, "failed start the task"); 
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}
	
	AOSUNLOCK(mLock);
	return true;
}


bool
AosJob::logJob(
		const OmnString &entry_key, 
		const AosRundataPtr &rdata)
{
	aos_assert_r(mJobDoc, false);

	OmnString docstr = "<joblog ";
	docstr << AOSTAG_PARENTC << "=\"" << mLogContainerObjid << "\" " 
		   << AOSTAG_OTYPE << "=\"" << AOSOTYPE_JOBLOG << "\" "
		   << AOSTAG_LOG_ENTRY_KEY << "=\"" << entry_key << "\" "
		   << AOSTAG_JOB_DOCID << "=\"" << mJobDocid << "\" "
		   << AOSTAG_REQUESTER << "=\"" << rdata->getUserid() << "\" "
		   << AOSTAG_JOB_STATUS << "=\"" << AosJobStatus::toStr(mStatus) << "\" "
		   << AOSTAG_PROGRESS << "=\"" << mJobProgress << "\" ";

	OmnString attr;
	set<OmnString>::iterator set_itr;
	for (set_itr = mNeedLogAttrs.begin(); set_itr != mNeedLogAttrs.end(); set_itr++)
	{
		attr = *set_itr;
		docstr << attr << "=\"" << mJobDoc->getAttrStr(attr) << "\" ";
	}

	docstr << "/>";
	//OmnScreen << docstr << endl;
	AosAddContainerLog(mLogContainerObjid, docstr, rdata);
	return true;
}

bool
AosJob::checkLogicTaskFinished(const AosRundataPtr &rdata)
{
	AosLogicTaskObjPtr logic_task;
	map<OmnString, AosLogicTaskObjPtr>::iterator logic_itr = mLogicTasks.begin();
	while (logic_itr != mLogicTasks.end())
	{
		logic_task = logic_itr->second;
		if (!logic_task->isFinished())
		{
			return false;
		}
		logic_itr++;
	}
	return true;
}

bool
AosJob::checkDataColFinished(const AosRundataPtr &rdata)
{
	return mDataColMgr->checkAllFinished(rdata);
}


bool
AosJob::checkJobFinishing(const AosRundataPtr &rdata)
{
	AOSLOCK(mLock);
	if (mVersion == 1)
	{
		startNextLogicTask(rdata, AosJobMgrObj::eNormal);
		bool rslt = scheduleTasks(rdata);
		aos_assert_rl(rslt, mLock, false);
	}

	bool logic_task_finished = checkLogicTaskFinished(rdata);
OmnScreen << "jozhi logic task finished: " << (logic_task_finished?"true":"false") << endl;
	bool col_mgr_finished = checkDataColFinished(rdata);
OmnScreen << "jozhi datacol mgr finished: " << (col_mgr_finished?"true":"false") << endl;
	if (!logic_task_finished || !col_mgr_finished)
	{
		AOSUNLOCK(mLock);
		return true;
	}

	bool rslt = commit(rdata);
	if (!rslt)
	{
		jobFailedLocked(rdata);
	}
	jobFinishedLocked(rdata);
	AOSUNLOCK(mLock);

	OmnTagFuncInfo << endl;
	return true;
}


bool
AosJob::jobFailedLocked(const AosRundataPtr &rdata)
{
	aos_assert_r(mJobDoc, false);

	//Jozhi 2015-12-04
	bool rslt = clearIILEntryMap(rdata);
	aos_assert_r(rslt, false);
	
	mStatus = AosJobStatus::eFail;
	OmnTagFuncInfo << "job status is fail: " << getJobDocid() << endl;

	AosModifyDocAttrStr(rdata, mJobDoc, AOSTAG_STATUS, AOSJOBSTATUS_FAIL);
	logJob(AOSJOBLOG_JOB_FAILED, rdata);
	
	AosJobObjPtr thisptr(this, true);
	AosJobMgr::getSelf()->jobFailed(thisptr, rdata);

	rdata->setArg1(AOSTAG_JOBSTATUS, "failed");
	rdata->setWorkingDoc(mJobDoc, false);
	AOSSYSLOG_CREATE(true, AOSSYSLOGNAME_JOB, rdata);

	rslt = jobStopAllTaskLocked(rdata);
	aos_assert_r(rslt, false);
	return true;
}


bool
AosJob::jobStopAllTaskLocked(const AosRundataPtr &rdata)
{
	bool rslt = false;
	map<OmnString, AosLogicTaskObjPtr>::iterator itr;
	for(itr = mLogicTasks.begin(); itr != mLogicTasks.end(); itr++)
	{
		rslt = (itr->second)->stopAllTaskProcess(rdata);
		aos_assert_r(rslt, false);
	}
	return true;
}

bool
AosJob::initSnapShots(const AosRundataPtr &rdata)
{
	aos_assert_r(mJobDoc, false);
	AosXmlTagPtr snapshot_tags = mJobDoc->getFirstChild("job_snapshots__n");
	if (snapshot_tags)
	{
		AosTaskDataType::E type;
		AosTaskDataObjPtr snapshot;
		u32 virtual_id = 0;
		AosXmlTagPtr snapshot_tag = snapshot_tags->getFirstChild();
		while(snapshot_tag)
		{
			snapshot = AosTaskData::serializeFromStatic(snapshot_tag, rdata);
OmnScreen << "initSnapShots, key: " << snapshot->getStrKey(rdata) << endl;
			SnapShotInfo info;
			info.mSnapTag = snapshot_tag;
			info.mSnapShot = snapshot;
			virtual_id = snapshot->getVirtualId();
			type = snapshot->getTaskDataType();
			if (type == AosTaskDataType::eIILSnapShot)
			{
				mIILSnapShots[virtual_id] = info;
			}
			else if (type == AosTaskDataType::eDocSnapShot)
			{
				mDocSnapShots[virtual_id] = info;
			}
			snapshot_tag = snapshot_tags->getNextChild();
		}
	}
	return true;
}

bool
AosJob::commit(const AosRundataPtr &rdata)
{
	OmnTagFuncInfo << "start to commit job snapshot" << endl;
	OmnScreen << "===============================commiting ...." << endl;
	AosIILClientObjPtr iil_client = AosIILClientObj::getIILClient();
	aos_assert_r(iil_client, false);
	AosDocClientObjPtr doc_client = AosDocClientObj::getDocClient();
	aos_assert_r(doc_client, false);
	AosTaskDataObjPtr snapshot;
	u64 task_docid = 0;
	u32 virtual_id = 0;
	u64 snapshot_id = 0;
	bool iscommit = false;
	bool rslt = false;
	AosTaskDataType::E type;
	AosDocType::E doctype = AosDocType::eNormalDoc;
	AosXmlTagPtr job_snapshot_tags = mJobDoc->getFirstChild("job_snapshots__n");
	if (job_snapshot_tags)
	{
		AosXmlTagPtr snapshot_tag = job_snapshot_tags->getFirstChild();
		while(snapshot_tag)
		{
			snapshot = AosTaskData::serializeFromStatic(snapshot_tag, rdata);
			aos_assert_r(snapshot, false);
			virtual_id = snapshot->getVirtualId();		
			snapshot_id = snapshot->getSnapShotId();
			iscommit = snapshot->getIsCommit();
			task_docid = snapshot->getTaskDocid();
			if (iscommit)
			{
				snapshot_tag = job_snapshot_tags->getNextChild();
				continue;
			}
			type = snapshot->getTaskDataType();
			if (type == AosTaskDataType::eIILSnapShot)
			{
				OmnTagFuncInfo << "commit iil snapshot" << endl;
				rslt = iil_client->commitSnapshot(virtual_id, snapshot_id, task_docid, rdata);
				aos_assert_r(rslt, false);
			}
			else if (type == AosTaskDataType::eDocSnapShot)
			{
				OmnTagFuncInfo << "commit doc snapshot" << endl;
				doctype = snapshot->getDocType();
				rslt = doc_client->commitSnapshot(rdata, virtual_id, doctype, snapshot_id, task_docid);
				aos_assert_r(rslt, false);
			}
			
			rslt = AosSnapshotIdMgr::getSelf()->deleteSnapshotId(snapshot_id, rdata);
			aos_assert_r(rslt, false);
			
			snapshot_tag->setAttr(AOSTAG_IS_COMMIT, true);
			snapshot_tag = job_snapshot_tags->getNextChild();
		}
	}

	OmnTagFuncInfo << endl;
	rslt = AosModifyDoc(mJobDoc, rdata);
	aos_assert_r(rslt, false);
	return true;
}

bool
AosJob::jobFinishedLocked(const AosRundataPtr &rdata)
{
	aos_assert_r(mJobDoc, false);
	cleanAllTaskDataOutPut(rdata);
	mFinishTime = OmnGetTime(AosLocale::eChina);
	i64 start_time = AosTimeUtil::str2EpochTime(mStartTime);
	i64 finish_time = AosTimeUtil::str2EpochTime(mFinishTime);
	OmnString take_time;
	take_time << (finish_time - start_time);
	mStatus = AosJobStatus::eFinish;
	OmnTagFuncInfo << "job status is finished: " << getJobDocid() << endl;
	mJobProgress = 100;
	mJobDoc->setAttr(AOSTAG_STATUS, AOSJOBSTATUS_FINISH);
	mJobDoc->setAttr(AOSTAG_JOB_PROGRESS, "100");
	mJobDoc->setAttr(AOSTAG_JOB_FINISHED_TIME, mFinishTime);
	mJobDoc->setAttr(AOSTAG_JOB_TAKE_TIME, take_time);
	AosModifyDoc(mJobDoc, rdata);

	logJob(AOSJOBLOG_JOB_FINISHED, rdata);

	AosJobObjPtr thisptr(this, true);
	AosJobMgr::getSelf()->jobFinished(thisptr, rdata);

	bool rslt = clearIILEntryMap(rdata);
	aos_assert_r(rslt, false);

	rdata->setArg1(AOSTAG_JOBSTATUS, "finish");
	rdata->setWorkingDoc(mJobDoc, false);
	AOSSYSLOG_CREATE(true, AOSSYSLOGNAME_JOB, rdata);
	if (!AosEvent::checkEvent1(0, mJobDoc, AOSEVENTTRIGGER_JOB_FINISHED, rdata))   
	{
		rdata->setError() << "Failed to Container Member Verification";
		AOSLOG_LEAVE(rdata);
		return 0;
	}
	OmnScreen << "Job Finished, finish time is : " << mFinishTime << ", take time is : " << take_time << " ==========" << endl;
	showJobInfo("finish");
	return true;
}


bool
AosJob::scheduleTasks(const AosRundataPtr &rdata)
{
	aos_assert_r(isStarted(), true);
	return AosTaskDriver::getSelf()->scheduleTasks();
}


bool
AosJob::scheduleNextTask(
		const AosTaskRunnerInfoPtr &run_info,
		const AosRundataPtr &rdata)
{
	aos_assert_r(mScheduler, false);
	AosJobObjPtr thisPtr(this, false);

	OmnTagFuncInfo << endl;
	bool rslt = mScheduler->nextTask(rdata, thisPtr, run_info); 
	return rslt;
}


AosLogicTaskObjPtr
AosJob::getLogicTask(
		const OmnString &logic_id,
		const AosRundataPtr &rdata)
{
	aos_assert_r(logic_id != "", 0);
	AOSLOCK(mLock);
	AosLogicTaskObjPtr logic_task = getLogicTaskLocked(logic_id, rdata);
	AOSUNLOCK(mLock);
	return logic_task;
}


AosLogicTaskObjPtr
AosJob::getLogicTaskLocked(
		const OmnString &logic_id,
		const AosRundataPtr &rdata)
{
	AosLogicTaskObjPtr logic_task;
	map<OmnString, AosLogicTaskObjPtr>::iterator itr = mLogicTasks.find(logic_id);
	if (itr == mLogicTasks.end()) 
	{
		if (mDataColMgr)
		{
			logic_task = mDataColMgr->getLogicTask(logic_id, rdata);
			if (logic_task)
				return logic_task;
		}
		AosSetErrorU(rdata, "logic_task_not_found:") << logic_id;
		OmnAlarm << rdata->getErrmsg() << enderr;
		return 0;
	}
	logic_task = itr->second;
	return logic_task;
}

	
void
AosJob::clear()
{
	mNeedLogAttrs.clear();
	mScheduler = 0;
	map<OmnString, AosLogicTaskObjPtr>::iterator logic_itr = mLogicTasks.begin();
	while (logic_itr != mLogicTasks.end())
	{
		logic_itr->second->clear();
		logic_itr++;
	}
	mLogicTasks.clear();
}


bool
AosJob::updateJobProgress(const AosRundataPtr &rdata)
{
	AOSLOCK(mLock);
	if (mStatus != AosJobStatus::eStart)
	{
		AOSUNLOCK(mLock);
		return true;	
	}

	int task_percents = 0;
	AosLogicTaskObjPtr logic_task;
	map<OmnString, AosLogicTaskObjPtr>::iterator task_itr = mLogicTasks.begin();
	while(task_itr != mLogicTasks.end())
	{
		logic_task = task_itr->second;
		task_percents += logic_task->getProgress();
		task_itr++;
	}

	map<OmnString, AosLogicTaskObjPtr> datacols;
	mDataColMgr->getAllDataCol(datacols);
	task_itr = datacols.begin();
	while(task_itr != datacols.end())
	{
		aos_assert_r(task_itr->second.getRef()>1, false);
		logic_task = task_itr->second;
		task_percents += logic_task->getProgress();
		task_itr++;
	}
	
	int job_percent = task_percents / (mLogicTasks.size() + datacols.size());

	if (job_percent < 0 || job_percent > 100)
	{
		AosSetErrorU(rdata, "job percent error:") << job_percent;
		OmnAlarm << rdata->getErrmsg() << enderr;
		AOSUNLOCK(mLock);
		return false;
	}
	
	if (mJobProgress >= job_percent)
	{
		AOSUNLOCK(mLock);
		return true;
	}

	mJobProgress = job_percent;
	if (mJobProgress >= 100) mJobProgress = 99;

	OmnString str;
	str << mJobProgress;
	OmnScreen << "job progress:" << str << endl;
	
	bool rslt = AosModifyDocAttrStr(rdata, mJobDoc, AOSTAG_JOB_PROGRESS, str);
	if (!rslt)
	{
		AosSetErrorU(rdata, "failed_modify_job_doc:") << mJobDoc->toString();
		OmnAlarm << rdata->getErrmsg() << enderr;
		AOSUNLOCK(mLock);
		return false;
	}

	AOSUNLOCK(mLock);
	return true;
}


bool
AosJob::stopJob(const AosRundataPtr &rdata)
{
	AOSLOCK(mLock);
	aos_assert_rl(!isStoped(), mLock, false);

	OmnTagFuncInfo << endl;
	if (isFinished()) 
	{
		AOSUNLOCK(mLock);
		return true;
	}
	if (isWaited())
	{
		mStatus = AosJobStatus::eStop; 
		mJobDoc->setAttr(AOSTAG_STATUS, AOSJOBSTATUS_STOP);
		AosModifyDoc(mJobDoc, rdata);
		AOSUNLOCK(mLock);
		return true;
	}

	aos_assert_rl(isStarted(), mLock, false);	
	mStatus = AosJobStatus::eStop;
	mJobDoc->setAttr(AOSTAG_STATUS, AOSJOBSTATUS_STOP);
	bool rslt = AosModifyDoc(mJobDoc, rdata);
	aos_assert_r(rslt, false);

	AosJobObjPtr thisptr(this, true);
	AosJobMgr::getSelf()->jobStoped(thisptr, rdata);

	rdata->setArg1(AOSTAG_JOBSTATUS, "stop");
	rdata->setWorkingDoc(mJobDoc, false);
	AOSSYSLOG_CREATE(true, AOSSYSLOGNAME_JOB, rdata);

	rslt = jobStopAllTaskLocked(rdata);
	AOSUNLOCK(mLock);
	return rslt;
}


bool
AosJob::setStatus(
		const AosJobStatus::E status,
		const AosRundataPtr &rdata)
{
	mStatus = status;
	OmnTagFuncInfo << "job status is : " << 
		status << " doc id is: " << getJobDocid() << endl;
	aos_assert_r(mJobDoc, false);
	AosModifyDocAttrStr(rdata, mJobDoc, AOSTAG_STATUS, AosJobStatus::toStr(mStatus));
	return true;
}


OmnString
AosJob::getJobObjid() const
{
	if (!mJobDoc) return "";
	return mJobDoc->getAttrStr(AOSTAG_OBJID);
}


u64
AosJob::getJobDocid() const
{
	if (!mJobDoc) return 0;
	return mJobDoc->getAttrU64(AOSTAG_DOCID, 0);
}


bool
AosJob::addTaskInfo(
		const AosTaskInfoPtr &info,
		const AosRundataPtr &rdata)
{
	aos_assert_r(info, false);
	vector<AosTaskInfoPtr> wait_task;
	wait_task.push_back(info);
	bool rslt = addTaskInfo(rdata, wait_task);
	return rslt;
}


bool
AosJob::addTaskInfo(
		const AosRundataPtr &rdata,
		const vector<AosTaskInfoPtr> &wait_list)
{
	aos_assert_r(mScheduler, false);
	mScheduler->addTasks(rdata, mJobDocid, wait_list);
	return true;
}

	
OmnString
AosJob::getJobENV(const OmnString &key)
{
	aos_assert_r(key != "", "");

	AOSLOCK(mLock);
	OmnString value = "";
	map<OmnString, OmnString>::iterator itr = mJobENV.find(key);
	if (itr != mJobENV.end())
	{
		value = itr->second;
	}
	AOSUNLOCK(mLock);
	return value;	
}


bool
AosJob::createScheduler(
		const AosXmlTagPtr &sdoc,
		const AosRundataPtr &rdata)
{
	AosXmlTagPtr scheduler_conf = sdoc->getFirstChild(AOSTAG_SCHEDULER);
	if (!scheduler_conf)
	{
		AosSetErrorU(rdata, "missing scheduler_conf:") << sdoc->toString();
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	mScheduler = AosTaskScheduler::createTaskScheduler(rdata, scheduler_conf);
	aos_assert_r(mScheduler, false);

	AosJobObjPtr thisPtr(this, true);
	mScheduler->setJob(thisPtr);
	return true;
}


bool
AosJob::createTasksByDoc(
		const AosJobMgrObj::OprId oprid,
		const AosRundataPtr &rdata)
{
	int startIdx = 0;
	AosXmlTagPtr contents = queryTaskDocs(startIdx, rdata);
	int total = contents->getAttrInt("total", 0);
	int num = contents->getAttrInt("num", 0);
	if (num <= 0)
	{
		return true;
	}
	bool rslt = createTasksByDoc(contents, oprid, rdata);
	aos_assert_r(rslt, false);

	vector<OmnThrdShellProcPtr> runners;
	OmnThrdShellProcPtr runner;
	while((total = total - num)> 0)
	{
		startIdx = startIdx + num;
		runner = OmnNew createQueryThrd(this, startIdx, oprid, rdata);
		runners.push_back(runner);
	}

	sgThreadPool->procSync(runners);
	aos_assert_rr(!mThrdShellError, rdata, false);

	return true;
}


bool
AosJob::createQueryThrd::run()
{
	AosXmlTagPtr contents;
	if (mLogicId != "")
	{
		contents = mJob->queryTaskDocs(mStartIdx, mLogicId, mRundata);
	}
	else
	{
		contents = mJob->queryTaskDocs(mStartIdx, mRundata);
	}
	aos_assert_r(contents, false);

	OmnTagFuncInfo << endl;

	int num = contents->getAttrInt("num", 0);
	if (num <= 0)
	{
		return true;
	}

	bool rslt = false;
	if (mCleanTaskData)
	{
		rslt = mJob->recycle(contents, mRundata);
		aos_assert_r(rslt, false);
	}
	else
	{
		rslt = mJob->createTasksByDoc(contents, mOprid, mRundata);
		aos_assert_r(rslt, false);
	}

	return true;
}


bool
AosJob::createTasksByDoc(
		const AosXmlTagPtr &contents,
		const AosJobMgrObj::OprId oprid,
		const AosRundataPtr &rdata)
{
	AosJobObjPtr thisptr(this, false);
	AosXmlTagPtr taskDoc = contents->getFirstChild(true);
	while(taskDoc)
	{
		AosXmlTagPtr clone_taskDoc = taskDoc->clone(AosMemoryCheckerArgsBegin);
		clone_taskDoc->removeAttr(AOSTAG_FULLDOC_SIGNATURE);

		OmnString logic_id = clone_taskDoc->getAttrStr(AOSTAG_LOGICID);
		aos_assert_r(logic_id != "", false);

		AosLogicTaskObjPtr logic_task;
		OmnString str_type = clone_taskDoc->getAttrStr(AOSTAG_TASK_TYPE);
		AosTaskType::E type = AosTaskType::toEnum(str_type);
		if (type == AosTaskType::eNormTask)
		{
			logic_task = getLogicTask(logic_id, rdata);
		}
		else if (type == AosTaskType::eReduceTask)
		{
			//Jozhi when restart job, to create real datacol
			AosXmlTagPtr tag = clone_taskDoc->getFirstChild("sdoc");
			aos_assert_r(tag, false);
			tag = tag->getFirstChild("dataproc");
			aos_assert_r(tag, false);
			tag = tag->getFirstChild("datacollector");
			aos_assert_r(tag, false);
			bool rslt = mDataColMgr->createDataCollector(mJobDocid, 
					thisptr, tag, logic_id, rdata);
			aos_assert_r(rslt, false);

			logic_task = mDataColMgr->getLogicTask(logic_id, rdata);
		}
		aos_assert_r(logic_task, false);

		AosJobObjPtr thisptr(this, false);
		bool rslt = logic_task->createTask(thisptr, clone_taskDoc, oprid, rdata);
		aos_assert_r(rslt, false);

		taskDoc = contents->getNextChild();
	}
	return true;
}


AosXmlTagPtr
AosJob::queryTaskDocs(
		const int startIdx, 
		const OmnString &logic_id,
		const AosRundataPtr &rdata)
{
	u64 job_docid = getJobDocid();
	OmnString task_ctnr = getTaskContainer();
	OmnString queryStr = "";
	queryStr << "<cmd psize=\"50\" reverse=\"true\" opr=\"retlist\" start_idx=\"" << startIdx << "\">"
		<< "<conds>"
		<< "<cond type=\"AND\">"

		<< "<cond type=\"arith\">"
		<< "<selector type=\"attr\" aname=\"" << AOSTAG_JOB_DOCID << "\"/>"
		<< "<cond type=\"arith\" zky_opr=\"eq\"><![CDATA[" << job_docid << "]]></cond>"
		<< "</cond>"

		<< "<cond type=\"arith\">"
		<< "<selector type=\"attr\" aname=\"" << AOSTAG_LOGICID << "\"/>"
		<< "<cond type=\"arith\" zky_opr=\"eq\"><![CDATA[" << logic_id << "]]></cond>"
		<< "</cond>"

		<< "<cond type=\"arith\">"
		<< "<selector type=\"attr\" aname=\"zky_pctrs\"/>"
		<< "<cond type=\"arith\" zky_opr=\"eq\"><![CDATA[" << task_ctnr << "]]></cond>"
		<< "</cond>"

		<< "</cond>"
		<< "</conds>"
		<< "<fnames><fname type=\"x\"><cname><![CDATA[xml]]></cname><oname><![CDATA[xml]]></oname></fname></fnames></cmd>";

	OmnTagFuncInfo << endl;

	AosXmlTagPtr cmd = AosXmlParser::parse(queryStr AosMemoryCheckerArgs);
	aos_assert_r(cmd, 0);

	AosQueryReqObjPtr query = AosQueryClient::getSelf()->createQuery(cmd, rdata);
	aos_assert_r(query && query->isGood(), 0);

	OmnString strData;
	bool rslt = query->procPublic(cmd, strData, rdata);
	aos_assert_r(rslt, 0);

	AosXmlTagPtr contents = AosXmlParser::parse(strData AosMemoryCheckerArgs);
	aos_assert_r(contents, 0);

	return contents;

}

AosXmlTagPtr
AosJob::queryTaskDocs(const int startIdx, const AosRundataPtr &rdata)
{
	u64 job_docid = getJobDocid();
	OmnString task_ctnr = getTaskContainer();
	OmnString queryStr = "";
	queryStr << "<cmd psize=\"50\" reverse=\"true\" opr=\"retlist\" start_idx=\"" << startIdx << "\">"
			 << "<conds>"
			 << "<cond type=\"AND\">"

			 << "<cond type=\"arith\">"
			 << "<selector type=\"attr\" aname=\"" << AOSTAG_JOB_DOCID << "\"/>"
			 << "<cond type=\"arith\" zky_opr=\"eq\"><![CDATA[" << job_docid << "]]></cond>"
			 << "</cond>"

			 << "<cond type=\"arith\">"
			 << "<selector type=\"attr\" aname=\"zky_pctrs\"/>"
			 << "<cond type=\"arith\" zky_opr=\"eq\"><![CDATA[" << task_ctnr << "]]></cond>"
			 << "</cond>"

			 << "</cond>"
			 << "</conds>" 
			 << "<fnames>"
			 << "<fname type=\"x\">"
			 << "<cname><![CDATA[xml]]></cname>"
			 << "<oname><![CDATA[xml]]></oname>"
			 << "</fname>"
			 << "</fnames></cmd>";

	AosXmlTagPtr cmd = AosXmlParser::parse(queryStr AosMemoryCheckerArgs);
	aos_assert_r(cmd, 0);

	AosQueryReqObjPtr query = AosQueryClient::getSelf()->createQuery(cmd, rdata);
	aos_assert_r(query && query->isGood(), 0);

	OmnString strData;
	bool rslt = query->procPublic(cmd, strData, rdata);
	aos_assert_r(rslt, 0);

	AosXmlTagPtr contents = AosXmlParser::parse(strData AosMemoryCheckerArgs);
	aos_assert_r(contents, 0);

	return contents;
}


bool
AosJob::checkStartJob()
{
	aos_assert_r(isStarted(), false);
	AosJobObjPtr thisptr(this, false);
	map<OmnString, AosLogicTaskObjPtr>::iterator itr = mLogicTasks.begin(); 
	while(itr != mLogicTasks.end())
	{
		(itr->second)->checkStartTask(thisptr, mRundata);
		itr++;
	}

	map<OmnString, AosLogicTaskObjPtr> datacols;
	mDataColMgr->getAllDataCol(datacols);
	itr = datacols.begin();
	while(itr != datacols.end())
	{
		(itr->second)->checkStartTask(thisptr, mRundata);
		itr++;
	}
	return true;
}


bool
AosJob::svrDeath(const int svr_id)
{
	aos_assert_r(isStarted(), false);
	AosJobObjPtr thisptr(this, false);
	map<OmnString, AosLogicTaskObjPtr>::iterator itr;
	for(itr = mLogicTasks.begin(); itr != mLogicTasks.end(); itr++)
	{
		bool rslt = (itr->second)->findFailedTaskBySvrId(thisptr, svr_id, mRundata);
		aos_assert_r(rslt, false);
	}
	return true;
}


bool
AosJob::returnSchedulerActiveTask(
		const int task_svr_id, 
		const u64 &task_docid,
		const AosRundataPtr &rdata)
{
	aos_assert_r(mScheduler, false);
	return mScheduler->returnNumActive(rdata, task_docid, task_svr_id);
}


bool
AosJob::recoverSchedulerActiveTask(
		const int task_svr_id, 
		const u64 &task_docid,
		const AosRundataPtr &rdata)
{
	aos_assert_r(mScheduler, false);
	return mScheduler->recoverNumActive(rdata, task_docid, task_svr_id);
}


bool
AosJob::removeWaitListByTaskId(const u64 &task_docid)
{
	aos_assert_r(mScheduler, false);
	return mScheduler->removeWaitListByTaskId(task_docid);
}


bool
AosJob::updateJobFailTaskNum(const AosRundataPtr &rdata)
{
	aos_assert_r(mJobDoc, false);
	u64 num = mJobDoc->getAttrU64(AOSTAG_TASK_FAILNUM, 0);
	OmnString nn;
	nn << ++num;
	AosModifyDocAttrStr(rdata, mJobDoc, AOSTAG_TASK_FAILNUM, nn);
	return true;
}


bool
AosJob::jobFailed(
		const AosRundataPtr &rdata,
		const bool run_cond)
{
	if (run_cond)
	{
		aos_assert_r(mJobDoc, false);
		u64 num = mJobDoc->getAttrU64(AOSTAG_TASK_FAILNUM, 0);
		if (num < AosJobMgr::eMaxFailTasks)
		{
			int num_physicals = AosGetNumPhysicals();
			int num_unusable = AosJobMgrObj::getJobMgr()->getNumUnusableProc();
			if (num_unusable * 100 < num_physicals * AosJobMgr::eProcUnusablePercent)
			{
				return false;
			}
			OmnScreen << "job failed job_docid: " << mJobDocid << " : num_unusable: " << num_unusable  << " : num_physicals:" << num_physicals << endl;
		}
		OmnScreen << "job failed" << mJobDocid << " , task_fail_num: " << num << endl;
	}
	OmnScreen << "job failed, job_docid:" << mJobDocid << endl;
	jobFailedLocked(rdata);
	return true;
}


bool
AosJob::cleanAllTaskDataOutPut(const AosRundataPtr &rdata)
{
	int startIdx = 0;
	AosXmlTagPtr contents = queryTaskDocs(startIdx, rdata);
	int total = contents->getAttrInt("total", 0);
	int num = contents->getAttrInt("num", 0);
	if (num <= 0)
	{
		return true;
	}
	bool rslt = recycle(contents, rdata);
	aos_assert_r(rslt, false);

	vector<OmnThrdShellProcPtr> runners;
	OmnThrdShellProcPtr runner;
	while((total = total - num)> 0)
	{
		startIdx = startIdx + num;
		runner = OmnNew createQueryThrd(this, startIdx, rdata);
		runners.push_back(runner);
	}
	sgThreadPool->procSync(runners);
	aos_assert_rr(!mThrdShellError, rdata, false);
	return true;
}


bool
AosJob::recycle(
		const AosXmlTagPtr &contents,
		const AosRundataPtr &rdata)
{
	vector<AosRecycle::RInfo> grinfo;
	AosXmlTagPtr taskDoc = contents->getFirstChild(true);
	while(taskDoc)
	{
		AosXmlTagPtr clone_taskDoc = taskDoc->clone(AosMemoryCheckerArgsBegin);
		clone_taskDoc->removeAttr(AOSTAG_FULLDOC_SIGNATURE);

		AosXmlTagPtr output_tag = clone_taskDoc->getFirstChild("output_data__n");
		if (output_tag)
		{
			AosXmlTagPtr taskdata_tag = output_tag->getFirstChild();
			while(taskdata_tag)
			{
				OmnString str_type = taskdata_tag->getAttrStr(AOSTAG_TYPE, "");
				AosTaskDataType::E type = AosTaskDataType::toEnum(str_type);
				if (type == AosTaskDataType::eOutPutDoc || 
						type == AosTaskDataType::eOutPutIIL)
				{
					AosRecycle::RInfo info;
					info.file_id = taskdata_tag->getAttrU64(AOSTAG_STORAGE_FILEID, 0);
					info.physicalid = taskdata_tag->getAttrInt(AOSTAG_PHYSICALID, -1);
					grinfo.push_back(info);
				}
				taskdata_tag = output_tag->getNextChild();
			}
		}
		taskDoc = contents->getNextChild();
	}

	if (grinfo.size() != 0)
	{
		AosRecycle::getSelf()->addRequest(grinfo, rdata);
	}
	return true;
}

bool
AosJob::createJobRunTime(
		const AosXmlTagPtr &job_doc,
		const AosXmlTagPtr &job_sdoc,
		const AosRundataPtr &rdata)
{
	mJobRunTime = OmnNew AosJobRunTime();
	return true;
}

bool
AosJob::initJob(
		const AosJobMgrObj::OprId id,
		const AosXmlTagPtr &jobdoc,
		const AosXmlTagPtr &sdoc, 
		const AosRundataPtr &rdata)
{
	aos_assert_r(jobdoc && sdoc, false);
	createJobRunTime(jobdoc, sdoc, rdata);
	createDataColMgr(rdata);

	mVersion = 1;
	u64 userid = rdata->getUserid();
	if (userid == 0)
	{
		AosSetErrorUser(rdata, "user_not_logged_in") << enderr;
		return false;
	}
	OmnString cid = rdata->getCid();
	if (cid == "")
	{
		AosSetErrorUser(rdata, "missing_cid") << enderr;
		return false;
	}
	mJobDoc = jobdoc;
	mJobSdoc = sdoc;
	mJobDocid = jobdoc->getAttrU64(AOSTAG_DOCID, 0);
	if (!mJobDocid)
	{
		AosSetErrorU(rdata, "missing job docid:") << jobdoc->toString();
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	mJobName = jobdoc->getAttrStr(AOSTAG_JOB_NAME);
	if (mJobName == "")
	{
		AosSetErrorU(rdata, "missing job name : ") << jobdoc->toString();
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	mSingleRun = jobdoc->getAttrBool(AOSTAG_SINGLERUN, true);
	mIsDeleteFile = sdoc->getAttrBool(AOSTAG_IS_DELETE_FILE, false);
	mIsPublic = sdoc->getAttrBool(AOSTAG_ISPUBLIC, true);
	mJobContainerObjid = sdoc->getAttrStr(AOSTAG_CONTAINER_OBJID);
	if (mJobContainerObjid == "")
	{
		mJobContainerObjid = AosObjid::composeUserJobContainerObjid(userid);
		aos_assert_rr(mJobContainerObjid != "", rdata, false);

		AosXmlTagPtr ctnr = AosCreateUserContainer(rdata, 
				mJobContainerObjid, "Private Job Container", cid);
		if (!ctnr)
		{
			AosSetErrorUser(rdata, "failed_create_user_job_container") << enderr;
			return false;
		}
	}

	AosXmlTagPtr dd = AosGetParentContainer(rdata, mJobContainerObjid, mIsPublic);
	aos_assert_r(dd, false);
	
	mLogContainerObjid = sdoc->getAttrStr(AOSTAG_LOG_CONTAINER, AOSCTNR_JOB_LOG_CONTAINER);
	if (mLogContainerObjid == "")
	{
		AosSetErrorU(rdata, "missing job log container objid:") << sdoc->toString();
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	dd = AosGetParentContainer(rdata, mLogContainerObjid, mIsPublic);
	aos_assert_r(dd, false);

	bool rslt = createJobENV(sdoc, rdata);
	aos_assert_r(rslt, false);

	rslt = createJobENV(jobdoc, rdata);
	aos_assert_r(rslt, false);

	OmnString job_docid;
	job_docid << mJobDocid;
	mJobENV[AOSTAG_JOB_DOCID] = job_docid;	

	mNeedLogAttrs.clear();
	mNeedLogAttrs.insert(AOSTAG_JOBNAME);
	mNeedLogAttrs.insert(AOSTAG_CTIME);
	mNeedLogAttrs.insert(AOSTAG_CT_EPOCH);
	OmnString logAttrs = sdoc->getAttrStr(AOSTAG_NEED_LOG_ATTRS);
	if (logAttrs != "")
	{
		OmnString attr;
		OmnStrParser1 parser(logAttrs, ",");
		while ((attr= parser.nextWord()) != "")
		{
			mNeedLogAttrs.insert(attr);
		}
	}

	rslt = createScheduler(sdoc, rdata);
	aos_assert_r(rslt, false);

	rslt = initSnapShots(rdata);
	aos_assert_r(rslt, false);

	rslt = createLogicTasks(rdata, sdoc);
	aos_assert_r(rslt, false);

	if (mLogicTasks.size() == 0)
	{
		AosSetErrorU(rdata, "no tasks:") << sdoc->toString();
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}
	return true;
}

bool
AosJob::tryScheduleAllMapTask(
		const AosJobMgrObj::OprId oprid,
		const OmnString &logic_id,
		const AosRundataPtr &rdata)
{
	aos_assert_r(oprid != AosJobMgrObj::eNormal, false);
	AosLogicTaskObjPtr logic_task = getLogicTaskLocked(logic_id, rdata);
	aos_assert_r(logic_task, false);
	return tryScheduleTaskPriv(oprid, logic_task, rdata);
}

bool
AosJob::tryScheduleAllReduceTask(
		const AosJobMgrObj::OprId oprid,
		const OmnString &logic_id,
		const AosRundataPtr &rdata)
{
	aos_assert_r(oprid != AosJobMgrObj::eNormal, false);
	AosLogicTaskObjPtr logic_task = mDataColMgr->getLogicTask(logic_id, rdata);
	aos_assert_r(logic_task, false);
	return tryScheduleTaskPriv(oprid, logic_task, rdata);
}

bool
AosJob::tryScheduleTaskPriv(
		const AosJobMgrObj::OprId oprid,
		const AosLogicTaskObjPtr &logic_task,
		const AosRundataPtr &rdata)
{
	AosTaskInfoPtr info;
	vector<AosTaskInfoPtr> wait_tasks;
	map<u64, AosTaskInfoPtr> info_map = logic_task->getAllTaskInfo();
	map<u64, AosTaskInfoPtr>::iterator info_itr = info_map.begin();
	while(info_itr != info_map.end())
	{
OmnScreen << "jozhi try schedule Task: " << info_itr->first << endl;
		info = info_itr->second;
		if (info->getStatus() == AosTaskStatus::eFinish)
		{
			info_itr++;
			continue;
		}

		if (info->getStatus() == AosTaskStatus::eStart)
		{
			if (oprid == AosJobMgrObj::eSwitchJobMaster)
			{
				OmnScreen << "switch job master, add info to scheduler, dice task_docid: " << info->getTaskDocid() << endl;
				info_itr++;
				continue;
			}
		}

		logic_task->setTaskInfoStatus(info, AosTaskStatus::eWait);
		wait_tasks.push_back(info);
		info_itr++;
	}
	logic_task->setStatus(AosTaskStatus::eStart);
	addTaskInfo(rdata, wait_tasks);
	return true;
}

bool
AosJob::recoverAllTask(
		const AosJobMgrObj::OprId oprid,
		const AosRundataPtr &rdata)
{
	map<OmnString, AosLogicTaskObjPtr>::iterator itr = mLogicTasks.begin();
	while(itr != mLogicTasks.end())
	{
		int startIdx = 0;
		AosXmlTagPtr contents = queryTaskDocs(startIdx, itr->first, rdata);
		int total = contents->getAttrInt("total", 0);
		int num = contents->getAttrInt("num", 0);
		if (num <= 0)
		{
			itr++;
			continue;
		}
		bool rslt = createTasksByDoc(contents, oprid, rdata);
		aos_assert_r(rslt, false);

		vector<OmnThrdShellProcPtr> runners;
		OmnThrdShellProcPtr runner;
		while((total = total - num)> 0)
		{
			startIdx = startIdx + num;
			runner = OmnNew createQueryThrd(this, startIdx, itr->first, oprid, rdata);
			runners.push_back(runner);
		}
		sgThreadPool->procSync(runners);
		aos_assert_rr(!mThrdShellError, rdata, false);
		rslt = tryScheduleAllMapTask(oprid, itr->first, rdata);
		aos_assert_r(rslt, false);
		itr++;
	}

	map<OmnString, AosLogicTaskObjPtr> datacols;
	mDataColMgr->getAllDataCol(datacols);
	itr = datacols.begin();
	while(itr != datacols.end())
	{
		int startIdx = 0;
		AosXmlTagPtr contents = queryTaskDocs(startIdx, itr->first, rdata);
		int total = contents->getAttrInt("total", 0);
		int num = contents->getAttrInt("num", 0);
		if (num <= 0)
		{
			itr++;
			continue;
		}
		bool rslt = createTasksByDoc(contents, oprid, rdata);
		aos_assert_r(rslt, false);

		vector<OmnThrdShellProcPtr> runners;
		OmnThrdShellProcPtr runner;
		while((total = total - num)> 0)
		{
			startIdx = startIdx + num;
			runner = OmnNew createQueryThrd(this, startIdx, itr->first, oprid, rdata);
			runners.push_back(runner);
		}
		sgThreadPool->procSync(runners);
		aos_assert_rr(!mThrdShellError, rdata, false);
		rslt = tryScheduleAllReduceTask(oprid, itr->first, rdata);
		aos_assert_r(rslt, false);
		itr++;
	}
	return true;
}


bool
AosJob::createDataColMgr(const AosRundataPtr &rdata)
{
	mDataColMgr = OmnNew AosDataCollectorMgr();
	return true;
}

bool
AosJob::createLogicTasks(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &sdoc)
{
	aos_assert_r(sdoc, false);
	AosXmlTagPtr tasks_tag = sdoc->getFirstChild("tasks");
	aos_assert_r(tasks_tag, false);
	AosLogicTaskObjPtr logic_task;
	AosXmlTagPtr task_tag = tasks_tag->getFirstChild(true);
	AosJobObjPtr this_ptr(this, false);
	while (task_tag)
	{
		AosLogicTaskObjPtr logic_task = AosLogicTask::createLogicTask(this_ptr, task_tag, rdata);
		aos_assert_r(logic_task, false);

		OmnString logic_id = logic_task->getLogicId();
		AOSLOCK(mLock);
		aos_assert_r(!mLogicTasks.count(logic_id), false);
		mLogicTasks[logic_id] = logic_task;

		vector<OmnString> triggerids = logic_task->getTriggerIds();
		if (triggerids.empty())
		{
			if (mLogicTaskSet.find(logic_id) == mLogicTaskSet.end())
			{
				mLogicTaskSet.insert(logic_id);
				mLogicRunQueue.push_back(logic_id);
			}
			AOSUNLOCK(mLock);
		}
		else 
		{
			AOSUNLOCK(mLock);
			for (size_t i=0; i<triggerids.size(); i++)
			{
				mDataColMgr->addTriggedTaskid(logic_id, triggerids[i], rdata);
			}
		}

		task_tag = tasks_tag->getNextChild();
	}
	return true;
}


bool
AosJob::datacollectorFinished(
		const AosRundataPtr &rdata, 
		const OmnString &datacol_id,
		vector<OmnString> &logic_taskid)
{
	
	AOSLOCK(mLock);
	bool trigged = false;
	for (size_t i=0; i<logic_taskid.size(); i++)
	{
		if (mLogicTasks[logic_taskid[i]]->checkTrigger(datacol_id))
		{
			trigged = true;
			if (mLogicTaskSet.find(logic_taskid[i]) == mLogicTaskSet.end())
			{
				mLogicTaskSet.insert(logic_taskid[i]);
				mLogicRunQueue.push_back(logic_taskid[i]);
			}
		}
	}

	if (trigged)
	{
		startNextLogicTask(rdata, AosJobMgrObj::eNormal);
		bool rslt = scheduleTasks(rdata);
		aos_assert_r(rslt, false);
	}
	AOSUNLOCK(mLock);
	return true;
}


bool 
AosJob::createMapTask(
		const AosJobMgrObj::OprId oprid,
		const AosLogicTaskObjPtr &logic_task,
		const AosRundataPtr &rdata)
{
	OmnScreen << "============================11111first to start map task: logic_id: " << logic_task->getLogicId() << endl;
	AosJobObjPtr this_ptr(this, false);
	bool rslt = logic_task->createMapTasks(rdata, this_ptr);
	aos_assert_r(rslt, false);
	return true;
}

bool 
AosJob::createReduceTask(
		const AosJobMgrObj::OprId oprid,
		const AosLogicTaskObjPtr &logic_task,
		const AosRundataPtr &rdata)
{
	AosJobObjPtr this_ptr(this, false);
	bool rslt = logic_task->createReduceTasks(rdata, this_ptr);
	aos_assert_r(rslt, false);
	return true;
}

bool
AosJob::triggerAllLogicTasks(
		const AosRundataPtr &rdata, 
		const AosJobMgrObj::OprId oprid)
{
	AosLogicTaskObjPtr logic_task = 0;
	AosJobObjPtr this_ptr(this, false);

	for (u32 i = 0; i < mLogicRunQueue.size(); i++)
	{
		logic_task = getLogicTaskLocked(mLogicRunQueue[i], rdata);
		aos_assert_r(logic_task, false);

		if (logic_task->getStatus() == AosTaskStatus::eStop)
		{
			//Jozhi 2014-05-09 dynamic to create map task
			//Jozhi 2014-05-12 this function has restart feathure
			bool rslt = createMapTask(oprid, logic_task, rdata);
			aos_assert_r(rslt, false);

			//in streaming mode, all the subtasks need to be created at the beginning
			rslt = createReduceTask(oprid, logic_task, rdata);
			aos_assert_r(rslt, false);

			//create output datasets. This is to prevent downstream
			//waiting for upstream tasks to start
			rslt = logic_task->createOutputDatasetDocs(rdata, this_ptr);
			aos_assert_r(rslt, false);

			AosTaskInfoPtr info;
			vector<AosTaskInfoPtr> wait_tasks;
			map<u64, AosTaskInfoPtr> info_map = logic_task->getAllTaskInfo();
			map<u64, AosTaskInfoPtr>::iterator info_itr = info_map.begin();
			while(info_itr != info_map.end())
			{
				info = info_itr->second;
				if (info->getStatus() == AosTaskStatus::eFinish)
				{
					info_itr++;
					continue;
				}

				if (info->getStatus() == AosTaskStatus::eStart)
				{
					aos_assert_r(oprid != AosJobMgrObj::eNormal, false);

					if (oprid == AosJobMgrObj::eSwitchJobMaster)
					{
						OmnScreen << "switch job master, add info to scheduler, dice task_docid: " << info->getTaskDocid() << endl;
						info_itr++;
						continue;
					}
				}

				int physical_id = info->getOptimalPhyId();
				if (physical_id < 0 || physical_id > AosGetNumPhysicals())
				{
					OmnScreen << "task docid: " << info->getTaskDocid() << endl;
					bool rslt = logic_task->taskStarted(info->getTaskDocid(), 0, 0, rdata);
					aos_assert_rl(rslt, mLock, false);
					rslt = logic_task->taskFinished(info->getTaskDocid(), rdata);
					aos_assert_rl(rslt, mLock, false);
					info_itr++;
					continue;
				}
				logic_task->setTaskInfoStatus(info, AosTaskStatus::eWait);
				wait_tasks.push_back(info);
				info_itr++;
			}
			logic_task->setStatus(AosTaskStatus::eStart);
			OmnTagFuncInfo << "start logic task: " << logic_task->getLogicId() << endl;
			addTaskInfo(rdata, wait_tasks);
		}
	}

	return true;
}

bool
AosJob::startNextLogicTask(
		const AosRundataPtr &rdata, 
		const AosJobMgrObj::OprId oprid)
{
	OmnTagFuncInfo << endl;
	AosLogicTaskObjPtr logic_task = 0;
	OmnTagFuncInfo << "logic run queue size is: " << mLogicRunQueue.size() << endl;
	for (u32 i = 0; i < mLogicRunQueue.size(); i++)
	{
		logic_task = getLogicTaskLocked(mLogicRunQueue[i], rdata);
		aos_assert_r(logic_task, false);

		if (logic_task->getType() == AosLogicTaskType::eMap &&
				logic_task->getStatus() == AosTaskStatus::eStart) 
		{
			logic_task = 0;
			continue;
		}

		if (logic_task->getStatus() == AosTaskStatus::eStop) 
		{
			break;
		}

		aos_assert_r(logic_task->isFinished(), false);
		logic_task = 0;
	}
	if (!logic_task) return true;

	aos_assert_r(logic_task->getStatus() == AosTaskStatus::eStop, false);

	showLogicTaskTotalStart(logic_task->getLogicId());
	bool rslt = createMapTask(oprid, logic_task, rdata);
	aos_assert_r(rslt, false);

	AosTaskInfoPtr info;
	vector<AosTaskInfoPtr> wait_tasks;
	map<u64, AosTaskInfoPtr> info_map = logic_task->getAllTaskInfo();
	map<u64, AosTaskInfoPtr>::iterator info_itr = info_map.begin();
	while(info_itr != info_map.end())
	{
		info = info_itr->second;
		if (info->getStatus() == AosTaskStatus::eFinish)
		{
			info_itr++;
			continue;
		}

		if (info->getStatus() == AosTaskStatus::eStart)
		{
			aos_assert_r(oprid != AosJobMgrObj::eNormal, false);

			if (oprid == AosJobMgrObj::eSwitchJobMaster)
			{
				OmnScreen << "switch job master, add info to scheduler, dice task_docid: " << info->getTaskDocid() << endl;
				info_itr++;
				continue;
			}
		}

		logic_task->setTaskInfoStatus(info, AosTaskStatus::eWait);
		wait_tasks.push_back(info);
		info_itr++;
	}
	logic_task->setStatus(AosTaskStatus::eStart);
	OmnTagFuncInfo << endl;
	addTaskInfo(rdata, wait_tasks);
	return true;
}

AosXmlTagPtr
AosJob::saveSnapShot(                                                        
		const AosXmlTagPtr &snapshot_tag,
		const AosRundataPtr &rdata)
{
	aos_assert_r(mJobDoc, 0);
	AosXmlTagPtr snapshot_tags = mJobDoc->getFirstChild("job_snapshots__n");
	if (!snapshot_tags)
	{
		snapshot_tags = mJobDoc->addNode1("job_snapshots__n");
	}
	aos_assert_r(snapshot_tags, 0);
	AosXmlTagPtr clone_node = snapshot_tag->clone(AosMemoryCheckerArgsBegin);
	return snapshot_tags->addNode(clone_node);
}

bool
AosJob::mergeSnapShotInfo(
		const AosTaskDataObjPtr &target_snap,
		const AosTaskDataObjPtr &merge_snap,
		const AosXmlTagPtr &snap_tag)
{
	set<OmnString> target_iilnames = target_snap->getIILNames();
	set<OmnString> merge_iilnames = merge_snap->getIILNames();
	set<OmnString>::iterator itr = merge_iilnames.begin();
	while(itr != merge_iilnames.end())
	{
		if (target_iilnames.find(*itr) == target_iilnames.end())
		{
			AosXmlTagPtr iil_tag = snap_tag->addNode1("iil");
			iil_tag->setAttr(AOSTAG_IILNAME, *itr);
		}
		itr++;
	}
	return true;
}

bool
AosJob::updateSnapShots(
		const AosXmlTagPtr &snapshot_tags,
		const AosRundataPtr &rdata)
{
	AosIILClientObjPtr iil_client = AosIILClientObj::getIILClient();
	aos_assert_r(iil_client, false);
	AosDocClientObjPtr doc_client = AosDocClientObj::getDocClient();
	aos_assert_r(doc_client, false);
	AosXmlTagPtr snapshot_tag = snapshot_tags->getFirstChild(true);
	AosTaskDataObjPtr snapshot = 0;
	while(snapshot_tag)
	{
		snapshot = AosTaskData::serializeFromStatic(snapshot_tag, rdata);
		OmnScreen << "================updateSnapShot: " << snapshot->getStrKey(rdata) << endl;
		aos_assert_r(snapshot, false);
		u32 virtual_id = snapshot->getVirtualId();
		AosTaskDataType::E type = snapshot->getTaskDataType();

		if (type == AosTaskDataType::eIILSnapShot)
		{
			map<u32, SnapShotInfo>::iterator itr = mIILSnapShots.find(virtual_id);
			if (itr == mIILSnapShots.end())
			{
				if (snapshot->getSnapShotId() != 0)
				{
					SnapShotInfo info;
					info.mSnapTag = saveSnapShot(snapshot_tag, rdata);
					info.mSnapShot = snapshot;
					mIILSnapShots[virtual_id] = info;
				}
			}
			else
			{
				AosTaskDataObjPtr target_task_data = (itr->second).mSnapShot;
				u64 target_snap_id = target_task_data->getSnapShotId();
				u64 merge_snap_id = snapshot->getSnapShotId();
				bool rslt = iil_client->mergeSnapshot(virtual_id, target_snap_id, merge_snap_id, rdata);
				rslt = AosSnapshotIdMgr::getSelf()->deleteSnapshotId(target_snap_id, rdata);
				aos_assert_r(rslt, false);
				rslt = mergeSnapShotInfo(target_task_data, snapshot, (itr->second).mSnapTag);
				aos_assert_r(rslt, false);
			}
		}
		else if (type == AosTaskDataType::eDocSnapShot)
		{
			map<u32, SnapShotInfo>::iterator itr = mDocSnapShots.find(virtual_id);
			if (itr == mDocSnapShots.end())
			{
				SnapShotInfo info;
				info.mSnapTag = saveSnapShot(snapshot_tag, rdata);
				info.mSnapShot = snapshot;
				mDocSnapShots[virtual_id] = info;
			}
			else
			{
				AosTaskDataObjPtr target_task_data = (itr->second).mSnapShot;
				u64 target_snap_id = target_task_data->getSnapShotId();
				u64 merge_snap_id = snapshot->getSnapShotId();
				AosDocType::E doctype = snapshot->getDocType();
				bool rslt = doc_client->mergeSnapshot(rdata, virtual_id, doctype, target_snap_id, merge_snap_id);
				rslt = AosSnapshotIdMgr::getSelf()->deleteSnapshotId(target_snap_id, rdata);
				aos_assert_r(rslt, false);
			}
		}
		snapshot_tag = snapshot_tags->getNextChild();
	}
	bool rslt = AosModifyDoc(mJobDoc, rdata);
	aos_assert_r(rslt, false);
	return true;
}

bool
AosJob::clearSnapShots()
{
	mDocSnapShots.clear();
	mIILSnapShots.clear();
	return true;
}

AosXmlTagPtr
AosJob::getSnapShots(const AosRundataPtr &rdata)
{
	AosXmlTagPtr snapshots = mJobDoc->getFirstChild("job_snapshots__n");
	aos_assert_r(snapshots, 0);
	return snapshots->clone(AosMemoryCheckerArgsBegin);
}

bool
AosJob::reset(const AosRundataPtr &rdata)
{
	map<OmnString, AosLogicTaskObjPtr>::iterator itr;
	AosLogicTaskObjPtr taskPtr;

	for(itr = mLogicTasks.begin(); itr != mLogicTasks.end(); itr++)
	{
		taskPtr = itr->second;
		taskPtr->reset(rdata);
	}
	startNextLogicTask(rdata, AosJobMgrObj::eNormal);

	return true;
}

AosValueRslt
AosJob::getRunTimeValue(
		const OmnString &key,
		const AosRundataPtr &rdata)
{
	AosValueRslt value;
	aos_assert_r(mJobRunTime, value);
	value = mJobRunTime->get(key, rdata);
	return value;
}

bool		
AosJob::setRunTimeValue(
		const OmnString &key,
		const AosValueRslt &value,
		const AosRundataPtr &rdata)
{
	aos_assert_r(mJobRunTime, 0);
	return mJobRunTime->set(key, value, rdata);
}


AosTaskInfoPtr
AosJob::getTaskInfoLocked(
		const u64 &task_docid,
		const AosRundataPtr &rdata)
{
	AosXmlTagPtr task_doc = AosGetDocByDocid(task_docid, rdata);
	aos_assert_r(task_doc, 0);
	OmnString logic_id = task_doc->getAttrStr(AOSTAG_LOGICID);
	aos_assert_r(logic_id != "", 0);
		
	AosLogicTaskObjPtr logic_task;
	OmnString str_type = task_doc->getAttrStr(AOSTAG_TASK_TYPE);
	AosTaskType::E type = AosTaskType::toEnum(str_type);
	if (type == AosTaskType::eNormTask)
	{
		logic_task = getLogicTaskLocked(logic_id, rdata);
	}
	else if (type == AosTaskType::eReduceTask)
	{
		logic_task = mDataColMgr->getLogicTask(logic_id, rdata);
	}
	aos_assert_r(logic_task, 0);
	aos_assert_r(task_docid != 0, 0);
	return logic_task->getTaskInfo(task_docid, rdata);
}

bool
AosJob::isValid(
		const u64 &task_docid,
		const u64 &stime,
		const AosRundataPtr &rdata)
{
	AosTaskInfoPtr info = getTaskInfoLocked(task_docid, rdata);
	aos_assert_r(info, false);

	if (info->getStatus() != AosTaskStatus::eStart)
	{
		OmnScreen << "start" << endl;
		OmnScreen << " isvalid isValid isValid task_docid: " << info->getTaskDocid() << ";" << endl;
		return false;
	}

	if (info->getTaskStartSTime() != stime) 
	{
		OmnScreen << " isvalid isValid isValid task_docid: " << info->getTaskDocid() << ";" <<info->getTaskStartSTime() << ":" << stime<< endl;
		OmnAlarm << "task_stime" << enderr;
		OmnScreen << "task_stime" << endl;
		return false; 
	}
	return true;
}

	
bool
AosJob::taskFailed(
		const AosXmlTagPtr &task_doc,
		const u64 &task_stime,
		const AosRundataPtr &rdata)
{
	// task
	//detect error reason
	//<task_doc...>
	//<error_reasons>
	//<reason zky_type="output_error" server_id="0"/>
	//<reason zky_type="input_error" server_id="2"/>
	//<reason zky_type="code_error" server_id="1"/>
	//</error_reasons>
	//</task_doc>
	//Get the last node, it's the current task fail reason. default is code error, if not detected.
	//1. input error: find all parent task(restart task), and all parent's children task,
	//   to clean resource(if it started, then to stop it, and clean all the output from
	//   the task, last to delete the task_doc.
	//2. code error: ...
	//3. ouput error: restart this task to another machine
	//All the task to restart before to check it's restart number of times.

	aos_assert_r(task_doc, false);
	u64 task_docid = task_doc->getAttrU64(AOSTAG_DOCID, 0);
	aos_assert_r(task_docid, false);
	
	AOSLOCK(mLock);
	if (!isValid(task_docid, task_stime, rdata)) 
	{
		AOSUNLOCK(mLock);
		return true;
	}
	OmnScreen << "Task Failed: " << task_docid << endl;

	//Check whether the job failure
	bool rslt = updateJobFailTaskNum(rdata);
	aos_assert_rl(rslt, mLock, false);

	rslt = jobFailed(rdata, true);
	if (rslt)
	{
		AOSUNLOCK(mLock);
		return true;
	}

	int error_num = 0;
OmnScreen << "=======================reason: " << error_num << endl;
	AosXmlTagPtr reason = AosTaskUtil::getErrorEntry(task_doc, error_num, rdata); 
	aos_assert_rl(reason, mLock, false);

	if (error_num > eMaxTaskFailNum) 
	{
		OmnScreen << "job failed, job_docid:" << mJobDocid  
			<< "error_num(" << error_num << ") > eMaxTaskFailNum(" << eMaxTaskFailNum << ")" << endl; 
		rslt = jobFailed(rdata, false);
		AOSUNLOCK(mLock);
		return rslt;
	}

	int serverid = reason->getAttrInt("zky_server_id", -1);
	aos_assert_rl(serverid >= 0, mLock, false);

	//stop fail task;
	AosTaskUtil::stopTaskProcess(serverid, task_docid, rdata);
	AosTaskInfoPtr info = getTaskInfoLocked(task_docid, rdata);
	aos_assert_rl(info->getTaskServerId() == serverid, mLock, false);
	aos_assert_rl(info->getStatus() == AosTaskStatus::eStart, mLock, false);
	OmnScreen << "taskFailed; task_docid:" << task_docid 
		<< "; error_type:" << reason->getAttrStr(AOSTAG_ZKY_TYPE)
	    << "; server_id:" << serverid 
	    << ";" << endl;
	switch (AosTaskErrorType::toEnum(reason->getAttrStr(AOSTAG_ZKY_TYPE)))
	{
	case AosTaskErrorType::eOutPutError:
	case AosTaskErrorType::eServerError:
	case AosTaskErrorType::eStorageError:
	case AosTaskErrorType::eCoreDumpError:
		 rslt = coreDumpErrorLocked(info, serverid, rdata);
		 aos_assert_r(rslt, false);
		 break;

	case AosTaskErrorType::eInPutError:
		 rslt = inputErrorLocked(info, task_docid, rdata);
		 aos_assert_rl(rslt, mLock, false);
		 break;

	case AosTaskErrorType::eCodeError:
		 rslt = codeErrorLocked(info, serverid, rdata);
		 aos_assert_rl(rslt, mLock, false);
		 break;

	default:
		 OmnAlarm << "" << enderr;
		 break;
	}
	AOSUNLOCK(mLock);
	return true;
}

bool
AosJob::coreDumpErrorLocked(
		const AosTaskInfoPtr &info,
		const int server_id,
		const AosRundataPtr &rdata)
{
	u64 task_docid = info->getTaskDocid();
	bool rslt = removeStartTask(info, rdata);
	aos_assert_r(rslt, false);

	info->setStatus(AosTaskStatus::eFail);
	info->setTaskStartSTime(0);
	rslt = returnSchedulerActiveTask(server_id, task_docid, rdata);	
	aos_assert_r(rslt, false);
	info->setOptimalPhyId(AosTaskUtil::nextPhysicalsId(server_id));
	info->incrementPriority();
	info->setStatus(AosTaskStatus::eWait);
	rslt = addTaskInfo(info, rdata);
	aos_assert_r(rslt, false);
	scheduleTasks(rdata);
	return true;
}


bool
AosJob::codeErrorLocked(
		const AosTaskInfoPtr &info,
		const int serverid,
		const AosRundataPtr &rdata)
{
	u64 task_docid = info->getTaskDocid();
	bool rslt = returnSchedulerActiveTask(serverid, task_docid, rdata);	
	aos_assert_rl(rslt, mLock, false);
	scheduleTasks(rdata);

	rslt = removeStartTask(info, rdata);
	aos_assert_r(rslt, false);

	info->setStatus(AosTaskStatus::eFail);
	info->setTaskStartSTime(0);
	OmnScreen << "job failed, job_docid:" << mJobDocid << "code Error serverid: " << serverid << " !!"<< endl; 
	return jobFailed(rdata, false);
}

bool
AosJob::inputErrorLocked(
		const AosTaskInfoPtr &info,
		const u64 &task_docid,
		const AosRundataPtr &rdata)
{
	set<u64> parent_task_docids;
	set<u64> child_task_docids;
	bool rslt = false;
	//Jozhi implement
	rslt = findRollBackTask(parent_task_docids, child_task_docids, task_docid, rdata);
	if (!rslt)
	{
		OmnScreen << "job failed, job_docid:" << mJobDocid  << " roll back failed!!" << endl; 
		return jobFailed(rdata, false);
	}

	if (parent_task_docids.size() == 0 && child_task_docids.size() == 0)
	{
		rslt = coreDumpErrorLocked(info, info->getTaskServerId(), rdata);
		return rslt;
	}
	//Jozhi implement
	rslt = killAllStartTask(parent_task_docids, child_task_docids, rdata);
	aos_assert_r(rslt, false);

	//Jozhi implement
	rslt = mDataColMgr->cleanDataCol(parent_task_docids, child_task_docids, rdata);
	aos_assert_r(rslt, false);

	//Jozhi implement
	rslt = cleanChildTask(child_task_docids, rdata);
	aos_assert_r(rslt, false);

	//Jozhi implement
	rslt = cleanParentTask(parent_task_docids, rdata);
	aos_assert_r(rslt, false);
	return true;
}

bool
AosJob::findRollBackTask(
		set<u64> &pdocids,
		set<u64> &cdocids,
		const u64 &task_docid,
		const AosRundataPtr &rdata)
{
	//input task data error rollback
	//current task has no child task
	pdocids.clear();
	AosXmlTagPtr task_doc = AosGetDocByDocid(task_docid, rdata);
	aos_assert_r(task_doc, false);

	bool rslt = findAllParentTask(pdocids, task_docid, rdata);
	aos_assert_r(rslt, false);

	set<u64>::iterator itr = pdocids.begin();
	while(itr != pdocids.end())
	{
		rslt = findAllChildTask(cdocids, *itr, rdata);
		aos_assert_r(rslt, false);
		itr++;
	}
	return true;
}

bool
AosJob::findAllParentTask(
		set<u64> &task_docids, 
		const u64 &task_docid, 
		const AosRundataPtr &rdata)
{
	AosXmlTagPtr task_doc = AosGetDocByDocid(task_docid, rdata);
	aos_assert_r(task_doc, false);
	AosXmlTagPtr sdoc = task_doc->getFirstChild("sdoc");
	AosXmlTagPtr files_tag;
	if (!sdoc)
	{
		//Jozhi 2015-03-17 will be implement
		//if this task input data from user's data, 
		//it's highest level task,will to fail job
		//it's not reduce task_doc
		//1. its first level task scann user data
		//2. its last level task save data to database
		AosXmlTagPtr tag = task_doc->getFirstChild("input_datasets");	
		aos_assert_r(tag, false);
		tag = tag->getFirstChild("dataset");
		OmnString trigger_type = tag->getAttrStr("trigger_type");
		if (trigger_type == "datacol")
		{
			tag = tag->getFirstChild("datascanner");
			aos_assert_r(tag, false);
			tag = tag->getFirstChild("dataconnector");
			aos_assert_r(tag, false);
			files_tag = tag->getFirstChild("files");
		}
		else
		{
			OmnAlarm << "no parent task, means this task not depends on any one" << enderr;
			return false;
		}
	}
	else
	{
		AosXmlTagPtr action = sdoc->getFirstChild("dataproc");
		aos_assert_r(action, false);
		files_tag = action->getFirstChild("files");
	}
	aos_assert_r(files_tag, false);

	u64 from_task_docid = 0;
	AosXmlTagPtr task_data_tag = files_tag->getFirstChild();
	while(task_data_tag)
	{
		from_task_docid = task_data_tag->getAttrU64(AOSTAG_FROM_TASK_DOCID, 0);
		aos_assert_r(from_task_docid != 0, false);
		if (task_docids.find(from_task_docid) == task_docids.end())
		{
			task_docids.insert(from_task_docid);
		}
		task_data_tag = files_tag->getNextChild();
	}
	return true;
}

bool
AosJob::findAllChildTask(
		set<u64> &task_docids, 
		const u64 &task_docid,
		const AosRundataPtr &rdata)
{
	if (mDeleteTaskDocs.find(task_docid) != mDeleteTaskDocs.end())
	{
		OmnScreen << "this task has delete, task_docid: " << task_docid << endl;
		return true;
	}
	AosXmlTagPtr task_doc = AosGetDocByDocid(task_docid, rdata);
	aos_assert_r(task_doc, false);
	AosXmlTagPtr output_tag = task_doc->getFirstChild("output_data__n");
	if (!output_tag)
	{
		return true;
	}
	u64 to_task_docid = 0;
	AosXmlTagPtr taskdata_tag = output_tag->getFirstChild();
	while(taskdata_tag)
	{
		AosXmlTagPtr to_task_docid_tag = taskdata_tag->getFirstChild();
		while(to_task_docid_tag)
		{
			OmnString str_docid = to_task_docid_tag->getNodeText();
			int reval = aos_value_atoull((char *)str_docid.data(), &to_task_docid);
			aos_assert_r(reval == 0, false);
			if (task_docids.find(to_task_docid) == task_docids.end())
			{
				task_docids.insert(to_task_docid);
				bool rslt = findAllChildTask(task_docids, to_task_docid, rdata);
				aos_assert_r(rslt, false);

			}
		}
		taskdata_tag = output_tag->getNextChild();
	}
	return true;
}



bool
AosJob::killAllStartTask(
			const set<u64> &p_docids,
			const set<u64> &c_docids,
			const AosRundataPtr &rdata)
{
	int server_id;
	bool rslt = false;
	AosTaskInfoPtr info;
	AosTaskStatus::E status;
	set<u64>::iterator itr = p_docids.begin();
	while(itr != p_docids.end())
	{
		info = getTaskInfoLocked(*itr, rdata);
		aos_assert_r(info, false);

		status = info->getStatus();
		if (status == AosTaskStatus::eStart)
		{
			server_id = info->getTaskServerId(); 
			rslt = AosTaskUtil::stopTaskProcess(server_id, *itr, rdata);
			aos_assert_r(rslt, false);
		}
		aos_assert_r(status == AosTaskStatus::eFinish, false);
		itr++;
	}


	// 1. kill child tasks if started(parent task, child task)
	// 2. remove "mStartTasks" map;
	// 3. remove wait task; 
	itr = c_docids.begin();
	while(itr != c_docids.end())
	{
		info = getTaskInfoLocked(*itr, rdata);
		if (!info)
		{
			itr++;
			continue;
		}
		status = info->getStatus();
		if (status == AosTaskStatus::eStart)
		{
			server_id = info->getTaskServerId(); 
			rslt = AosTaskUtil::stopTaskProcess(server_id, *itr, rdata);
			aos_assert_r(rslt, false);
			removeStartTask(info, rdata);
		}

		if (status == AosTaskStatus::eWait)
		{
			rslt = removeWaitListByTaskId(*itr);
			aos_assert_r(rslt, false);
			aos_assert_r(info->getStatus() == AosTaskStatus::eWait, false);
			info->setStatus(AosTaskStatus::eStop);
		}
		itr++;
	}
	scheduleTasks(rdata);
	return true;
}


bool
AosJob::cleanChildTask(
		const set<u64> &child_task_docids,
		const AosRundataPtr &rdata)
{
	set<u64>::iterator itr = child_task_docids.begin();
	while(itr != child_task_docids.end())
	{
		if (mDeleteTaskDocs.find(*itr) != mDeleteTaskDocs.end())
		{
			OmnScreen << "this task has delete, task_docid: " << *itr << endl;
			itr++;
			continue;
		}

		AosXmlTagPtr task_doc = AosGetDocByDocid(*itr, rdata);
		aos_assert_r(task_doc, false);
		AosXmlTagPtr output_tag = task_doc->getFirstChild("output_data__n");
		if (!output_tag)
		{
			OmnScreen << "taskFailed; input_error; " 
				<< "delete child_task_docid:" << *itr
				<< ";" << endl;
			cleanChildTaskInfoLocked(*itr, rdata);
			itr++;
			continue;
		}
		AosXmlTagPtr taskdata_tag = output_tag->getFirstChild();
		while(taskdata_tag)
		{
			OmnString str_type = taskdata_tag->getAttrStr(AOSTAG_TYPE, "");
			AosTaskDataType::E type = AosTaskDataType::toEnum(str_type);
			if (type == AosTaskDataType::eOutPutIIL)
			{
				u64 file_id = taskdata_tag->getAttrU64(AOSTAG_STORAGE_FILEID, 0);
				int physicalid = taskdata_tag->getAttrInt(AOSTAG_PHYSICALID, -1);
				bool svr_death = false;
OmnScreen << "===================delete file cleanChildTask, file_id : " << file_id << " physicalid: " << physicalid << endl;
				AosNetFileCltObj::deleteFileStatic(
					file_id, physicalid, svr_death, rdata.getPtr());
				if (svr_death) AosRecycle::getSelf()->diskRecycle(physicalid, file_id, rdata);
				OmnScreen << "taskFailed; input_error; " 
					<< "delete child_task_docid:" << *itr
					<< "; clean file; physicalid:" << physicalid 
					<< "; file_id:" << file_id 
					<< "; svr_death:" << (svr_death?"true":"false")
					<< endl;
			}
			taskdata_tag = output_tag->getNextChild();
		}
		cleanChildTaskInfoLocked(*itr, rdata);
		itr++;
	}
	return true;
}

bool
AosJob::cleanChildTaskInfoLocked(
		const u64 &task_docid,
		const AosRundataPtr &rdata)
{
OmnScreen << "to cleanChildTaskInfoLocked===========================" << task_docid << endl;
	bool rslt = false;

	AosTaskInfoPtr info = getTaskInfoLocked(task_docid, rdata);
	aos_assert_r(info, false);

	aos_assert_r(info->getStatus() != AosTaskStatus::eWait, false);

	if (info->getStatus() == AosTaskStatus::eStart)
	{
OmnScreen << "===========================start" << task_docid << endl;
		int serverid = info->getTaskServerId();
		rslt = returnSchedulerActiveTask(serverid, task_docid, rdata);	
		aos_assert_r(rslt, false);
	}

	if (info->getStatus() == AosTaskStatus::eFinish)
	{
OmnScreen << "===========================finish" << task_docid << endl;
		removeStartTask(info, rdata);
	}

	if (info->getStatus() == AosTaskStatus::eFail)
	{
OmnScreen << "===========================fail" << task_docid << endl;
		removeStartTask(info, rdata);
	}

	rslt = clearTaskInfo(info, rdata);
	aos_assert_r(rslt, false);
	rslt = AosDeleteDocByDocid(task_docid, rdata);
	aos_assert_r(rslt, false);

	mDeleteTaskDocs.insert(task_docid);
	return true;
}

bool
AosJob::cleanParentTask(
		const set<u64> &parent_task_docids,
		const AosRundataPtr &rdata)
{
	bool rslt = false;
	set<u64>::iterator itr = parent_task_docids.begin();
	while(itr != parent_task_docids.end())
	{
		rslt = cleanTaskDocOutPut(*itr, rdata);
		aos_assert_r(rslt, false);

		rslt = reStartParentTaskLocked(*itr, rdata);
		aos_assert_r(rslt, false);
		itr++;
	}
	return true;
}

bool
AosJob::cleanTaskDocOutPut(
		const u64 &task_docid,
		const AosRundataPtr &rdata)
{
	AosXmlTagPtr task_doc = AosGetDocByDocid(task_docid, rdata);
	aos_assert_r(task_doc, false);

	AosXmlTagPtr output_tag = task_doc->getFirstChild("output_data__n");
	if(!output_tag) return false;

	task_doc->setAttr(AOSTAG_STATUS, AOSTASKSTATUS_STOP);
	task_doc->setAttr(AOSTAG_START_STAMP, "0");

	AosXmlTagPtr taskdata_tag = output_tag->getFirstChild();
	while(taskdata_tag)
	{
		OmnString str_type = taskdata_tag->getAttrStr(AOSTAG_TYPE, "");
		AosTaskDataType::E type = AosTaskDataType::toEnum(str_type);
		if (type == AosTaskDataType::eOutPutIIL)
		{
			u64 file_id = taskdata_tag->getAttrU64(AOSTAG_STORAGE_FILEID, 0);
			int physicalid = taskdata_tag->getAttrInt(AOSTAG_PHYSICALID, -1);
			bool svr_death = false;
OmnScreen << "===================delete file cleanTaskDocOutPut, file_id : " << file_id << " physicalid: " << physicalid << endl;
			AosNetFileCltObj::deleteFileStatic(
				file_id, physicalid, svr_death, rdata.getPtr());
			if (svr_death) AosRecycle::getSelf()->diskRecycle(physicalid, file_id, rdata);
			OmnScreen << "taskFailed; input_error; or output_error;" 
					<< "clean task doc out put; task_docid:" << task_docid
					<< "; clean file; physicalid:" << physicalid 
					<< "; file_id:" << file_id 
					<< "; svr_death:" << (svr_death?"true":"false")
					<< endl;
		}
		taskdata_tag = output_tag->getNextChild();
	}
	task_doc->removeNode(output_tag);
OmnScreen << "================================modify task doc: " << task_docid << endl;
	rdata->setOk();
	bool rslt = AosModifyDoc(task_doc, rdata);
	aos_assert_r(rslt, false);
	//Jozhi 2015-03-18
	//if reduce task will insert to active task list
	OmnString str_type = task_doc->getAttrStr(AOSTAG_TASK_TYPE);
	AosTaskType::E type = AosTaskType::toEnum(str_type);
	if (type == AosTaskType::eReduceTask)
	{
		OmnString logic_id = task_doc->getAttrStr(AOSTAG_LOGICID);
		AosLogicTaskObjPtr logic_task = mDataColMgr->getLogicTask(logic_id, rdata);
		aos_assert_r(logic_task, false);
		logic_task->insertActiveTask(task_doc);
	}

	return true;
}



bool
AosJob::reStartParentTaskLocked(
		const u64 &task_docid,
		const AosRundataPtr &rdata)
{
	AosTaskInfoPtr info = getTaskInfoLocked(task_docid, rdata);
	aos_assert_r(info, false);
	aos_assert_r(info->getStatus() == AosTaskStatus::eFinish, false);
	removeStartTask(info, rdata);
	bool rslt = false;
	AosJobObjPtr thisptr(this, false);
	if (info->getTaskType() == AosTaskType::eNormTask)
	{
		AosLogicTaskObjPtr logic_task = getLogicTaskLocked(info->getLogicId(), rdata);
		aos_assert_r(logic_task, false);

		rslt = logic_task->reStartLogicTask(thisptr, rdata);
		aos_assert_r(rslt, false);
	}
	else if (info->getTaskType() == AosTaskType::eReduceTask)
	{
		AosLogicTaskObjPtr logic_task = mDataColMgr->getLogicTaskLocked(info->getLogicId(), rdata);
		aos_assert_r(logic_task, false);
		rslt = logic_task->reStartDataCol(thisptr, rdata);
		aos_assert_r(rslt, false);
	}
	info->incrementPriority();
	info->setStatus(AosTaskStatus::eWait);
	addTaskInfo(info, rdata);
	return true;
}


bool
AosJob::removeStartTask(
		const AosTaskInfoPtr &info,
		const AosRundataPtr &rdata)
{
	u64 task_docid = info->getTaskDocid();
	AosXmlTagPtr task_doc = AosGetDocByDocid(task_docid, rdata);
	aos_assert_r(task_doc, false);
	OmnString logic_id = info->getLogicId();
	AosLogicTaskObjPtr logic_task;
	OmnString str_type = task_doc->getAttrStr(AOSTAG_TASK_TYPE);
	AosTaskType::E type = AosTaskType::toEnum(str_type);
	if (type == AosTaskType::eNormTask)
	{
		logic_task = getLogicTaskLocked(logic_id, rdata);
	}
	else if (type == AosTaskType::eReduceTask)
	{
		logic_task = mDataColMgr->getLogicTask(logic_id, rdata);
	}

	aos_assert_r(logic_task, false);
	logic_task->removeStartTask(info);
	return true;
}

bool
AosJob::clearTaskInfo(
		const AosTaskInfoPtr &info,
		const AosRundataPtr &rdata)
{
	u64 task_docid = info->getTaskDocid();
	AosXmlTagPtr task_doc = AosGetDocByDocid(task_docid, rdata);
	aos_assert_r(task_doc, false);
	OmnString logic_id = info->getLogicId();
	AosLogicTaskObjPtr logic_task;
	OmnString str_type = task_doc->getAttrStr(AOSTAG_TASK_TYPE);
	AosTaskType::E type = AosTaskType::toEnum(str_type);
	if (type == AosTaskType::eNormTask)
	{
		logic_task = getLogicTaskLocked(logic_id, rdata);
	}
	else if (type == AosTaskType::eReduceTask)
	{
		logic_task = mDataColMgr->getLogicTask(logic_id, rdata);
	}
	aos_assert_r(logic_task, false);
	logic_task->clearTaskInfo(info);
	return true;
}


bool
AosJob::addRestartFinishResources(
				const AosXmlTagPtr &task_doc,
				const AosRundataPtr &rdata)
{
	// job and task
	aos_assert_r(task_doc, false);
	AosXmlTagPtr entry;
	AosXmlTagPtr tags = task_doc->getFirstChild("output_data__n");
	if (tags)
	{
OmnScreen << "jozhi addRestartFinishResources: " << tags->toString() << endl;
		entry = tags->getFirstChild(true);
		AosTaskDataObjPtr task_data;
		AosTaskDataType::E type;
		OmnString key;
		while(entry)
		{
			type = AosTaskDataType::toEnum(entry->getAttrStr(AOSTAG_TYPE));
			if (type == AosTaskDataType::eOutPutIIL)
			{
				task_data = AosTaskData::serializeFromStatic(entry, rdata);
				key = task_data->getStrKey(rdata);
				aos_assert_r(mOutPutResources.find(key) == mOutPutResources.end(), false);
				mOutPutResources[key] = entry;
			}
			entry = tags->getNextChild();
		}
	}
	return true;
}

bool
AosJob::addRestartReduceResources(
		const AosXmlTagPtr &task_doc,
		const AosRundataPtr &rdata)
{
	// job
	aos_assert_r(task_doc, false);
	AosXmlTagPtr sdoc = task_doc->getFirstChild("sdoc");
	AosXmlTagPtr files;
	if (!sdoc)
	{
		AosXmlTagPtr tag = task_doc->getFirstChild("input_datasets");	
		aos_assert_r(tag, false);
		tag = tag->getFirstChild("dataset");
		OmnString trigger_type = tag->getAttrStr("trigger_type");
		if (trigger_type == "datacol")
		{
			tag = tag->getFirstChild("datascanner");
			aos_assert_r(tag, false);
			tag = tag->getFirstChild("dataconnector");
			aos_assert_r(tag, false);
			files= tag->getFirstChild("files");
		}
	}
	else
	{
		AosXmlTagPtr dataproc = sdoc->getFirstChild("dataproc");
		aos_assert_r(dataproc, false);
		files= dataproc->getFirstChild("files");
	}
	if (!files)
	{
		return true;
	}
OmnScreen << "jozhi addRestartReduceResources: " << files->toString() << endl;

	//Jozhi this files child may be AosCompNetFile
	AosTaskDataObjPtr task_data;
	AosTaskDataType::E type;
	OmnString key;
	AosXmlTagPtr entry = files->getFirstChild(true);
	while(entry)
	{
		AosXmlTagPtr sub_file = entry->getFirstChild(true);
		if (sub_file)
		{
			while(sub_file)
			{
				type = AosTaskDataType::toEnum(sub_file->getAttrStr(AOSTAG_TYPE));
				if (type == AosTaskDataType::eOutPutIIL)
				{
					int64_t pos = sub_file->getAttrInt64(AOSTAG_START_POS, -1);
					if (pos > 0)
					{
						entry = sub_file->getNextChild();
						continue;
					}
					task_data = AosTaskData::serializeFromStatic(sub_file, rdata);
					key = task_data->getStrKey(rdata);
					aos_assert_r(mReduceResources.find(key) == mReduceResources.end(), false);
					mReduceResources[key] = entry;
				}

				sub_file = entry->getNextChild();
			}
		}
		else
		{
			type = AosTaskDataType::toEnum(entry->getAttrStr(AOSTAG_TYPE));
			if (type == AosTaskDataType::eOutPutIIL)
			{
				int64_t pos = entry->getAttrInt64(AOSTAG_START_POS, -1);
				if (pos > 0)
				{
					entry = files->getNextChild();
					continue;
				}
				task_data = AosTaskData::serializeFromStatic(entry, rdata);
				key = task_data->getStrKey(rdata);
				aos_assert_r(mReduceResources.find(key) == mReduceResources.end(), false);
				mReduceResources[key] = entry;
			}
		}
		entry = files->getNextChild();
	}
	return true;
}


bool
AosJob::clearIILEntryMap(const AosRundataPtr &rdata)
{
	AosIILEntryMapMgr::clear();

	//Jozhi 2015-03-20
	//may be new thread shell to run thease trans
	vector<OmnThrdShellProcPtr> runners;
	OmnThrdShellProcPtr runner;
	int num_physicals = AosGetNumPhysicals();
	for (int i=0; i<num_physicals; i++)
	{
		runner = OmnNew SendClearIILEntryMapThrd(i, rdata);
		runners.push_back(runner);
	}

	vector<u32> cubids = AosGetTotalCubeIds();
	for (size_t i=0; i<cubids.size(); i++)
	{
		runner = OmnNew SendClearCubeIILEntryMapThrd(cubids[i], rdata);
		runners.push_back(runner);
	}

	sgThreadPool->procSync(runners);
	aos_assert_rr(!mThrdShellError, rdata, false);
	return true;
}
		
		
bool
AosJob::SendClearIILEntryMapThrd::run()
{
	AosBuffPtr resp_buff;
	bool timeout = false;
	AosTransPtr trans = OmnNew AosSendClearIILEntryMapTrans(mPhysicalId);
	AosSendTrans(mRundata, trans, timeout, resp_buff);
	aos_assert_r(resp_buff && resp_buff->getU8(0), false);
	return true;
}


bool
AosJob::SendClearCubeIILEntryMapThrd::run()
{
	AosBuffPtr resp_buff;
	bool timeout = false;
	AosTransPtr trans = OmnNew AosClearCubeIILEntryMapTrans(mCubeId);
	AosSendTrans(mRundata, trans, timeout, resp_buff);
	aos_assert_r(resp_buff && resp_buff->getU8(0), false);
	return true;
}


u32							
AosJob::getNumSlots() const
{
	return mScheduler->getNumSlots();
}


//for log
void 
AosJob::showJobInfo(
		const OmnString &action)
{
	AosJobLog(mJobDocid) << "Act:" << action << " JOB:" << mJobName << endlog;
}


void
AosJob::showLogicTaskTotalStart(
		const OmnString &logic_id)
{
	AosJobLog(mJobDocid) << "Act:start LTSK:" << logic_id << endlog;
}

