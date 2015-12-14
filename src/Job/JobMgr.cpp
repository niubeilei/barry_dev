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
// How Job Communicates
// It uses AosSendCommand(module_id, server_id, buff, rdata) to send 
// messages to a remote server. In most cases, JobMgr needs to communicate
// with the JobMgr on a remote machine. In this case, 'module_id' is set
// to eJobMgr and 'server_id' is set to the target server. The message
// contents are in 'buff'. 
//
// Modification History:
// 04/26/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "Job/JobMgr.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "API/AosApi.h"
#include "SEInterfaces/DocClientObj.h"
#include "DocServer/ProcNames.h"
#include "DocServer/DocSvr.h"
#include "Job/Job.h"
#include "JobTrans/AllJobTrans.h"
#include "NetworkMgrUtil/NetMgrUtil.h"
#include "NetworkMgrUtil/NetCommand.h"
#include "NetworkMgrUtil/CommandRespCode.h"
#include "NetworkMgrUtil/CommandListener.h"
#include "Porting/Sleep.h"
#include "Rundata/Rundata.h"
#include "SEInterfaces/IILMgrObj.h"
#include "SEInterfaces/SeLogClientObj.h"
#include "SEInterfaces/SmartDocObj.h"
#include "SEInterfaces/TaskDataObj.h"
#include "SEInterfaces/ObjMgrObj.h"
#include "SEInterfaces/DataFieldType.h"
#include "SEInterfaces/DataRecordType.h"
#include "SEUtil/SysLogName.h"
#include "SingletonClass/SingletonImpl.cpp"
#include "Thread/CondVar.h"
#include "UtilTime/TimeUtil.h"
#include "XmlUtil/XmlTag.h"
#include "TaskUtil/TaskDriver.h"
#include "JobTrans/GetTaskRunnerInfoTrans.h"
#include "JobTrans/UpdateTaskOutPutTrans.h"
#include "XmlUtil/SeXmlParser.h"
#include "JobTrans/CreateSizeIdByDocTrans.h"
#include "JobTrans/CreateSizeIdByDocidTrans.h"

#include "Debug/Debug.h"

static OmnString	sgTimerSdocObjid = "system_runjob_timer";
static int			sgSleepTime = 60 * 60;


OmnSingletonImpl(AosJobMgrSingleton,
                 AosJobMgr,
                 AosJobMgrSelf,
                "AosJobMgr");


AosJobMgr::AosJobMgr()
:
mLock(OmnNew OmnMutex()),
mCondVar(OmnNew OmnCondVar()),
mRundata(OmnNew AosRundata(AosMemoryCheckerArgsBegin)),
mThreadStatus1(false),
mThreadStatus2(false),
mThreadStatus3(false),
mIsRunJob(false)
{
	OmnThreadedObjPtr thisPtr(this, false);
	mThread = OmnNew OmnThread(thisPtr, "JobMgrThrd", 0, true, true, __FILE__, __LINE__);
	mTimerThread = OmnNew OmnThread(thisPtr, "JobMgrTimerThrd", 1, true, true, __FILE__, __LINE__);
	
	AosCommandRunnerObjPtr cmd_runner(this, false);
	AosCommandRunnerObj::registerCommandRunner(AosModuleId::eJobMgr, cmd_runner);
}


AosJobMgr::~AosJobMgr()
{
}


bool
AosJobMgr::start()
{
	mThread->start();
	return true;
}


bool
AosJobMgr::config(const AosXmlTagPtr &config)
{
	aos_assert_r(config, false);

	AosXmlTagPtr job_cfg = config->getFirstChild("job_mgr");
	aos_assert_r(job_cfg, false);

	mSafeDiskFreeSize = job_cfg->getAttrU64("safe_disk_free_size", 10000000000ULL);
	aos_assert_r(mSafeDiskFreeSize > 0, false);

	AosXmlTagPtr disk = job_cfg->getFirstChild("disk");
	OmnString disk_name;
	while (disk)
	{
		disk_name = disk->getNodeText();
		mDiskNameSet.insert(disk_name);
		disk = job_cfg->getNextChild("disk");
	}

	aos_assert_r(mDiskNameSet.size() > 0, false);

	return true;
}


bool
AosJobMgr::stop()
{
    return true;
}


bool	
AosJobMgr::threadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread)
{
	switch (thread->getLogicId())
	{
	case 0 :
		 return mainThreadFunc(state, thread);

	case 1 :
		 return timerThreadFunc(state, thread);

	default :
		 break;
	}

	OmnShouldNeverComeHere;
	return false;
}


//Jozhi 2013-09-23
bool	
AosJobMgr::mainThreadFunc(
		OmnThrdStatus::E &state,
		const OmnThreadPtr &thread)
{
	AosJobObjPtr job;
	u64 job_docid = 0;
	AosXmlTagPtr job_doc;
	AosXmlTagPtr job_sdoc;
	AosRundataPtr rdata;
	OprId id = eInvalid;
	OmnString args = "";
    while (state == OmnThrdStatus::eActive)
	{
		mLock->lock();
		job = 0;
		if (mWaitJobInfos.size() <= 0)
		{
			mCondVar->wait(mLock);
			mThreadStatus1 = true;
			mLock->unlock();
			continue;
		}

		if (mStartJobs.size() >= eMaxStartJobs)
		{
			mCondVar->wait(mLock);
			mThreadStatus1 = true;
			mLock->unlock();
			continue;
		}

		if (hasSingleStartJob())
		{
			mCondVar->wait(mLock);
			mThreadStatus1 = true;
			mLock->unlock();
			continue;
		}

		mThreadStatus1 = true;

		JobInfo info = mWaitJobInfos.front();
		mWaitJobInfos.pop_front();

		job_docid = info.mJobDocid;
		rdata = info.mRundata;
		id = info.mOpr;
		job_doc = info.mJobDoc;
		job_sdoc = info.mJobSdoc;
		args = info.mArgs;

		OmnScreen << "add job to jobmgr, docid:" << job_docid << endl;
		OmnTagFuncInfo << "create a new job to run, docid:" << job_docid << endl;
		job = createJob(id, job_doc, job_sdoc, rdata);
		if (!job)
		{
			OmnAlarm << "can not create job"  << enderr;
			AosSetEntityError(rdata, "jobmgr_can_not_create_job", "JobMgr", "JobMgr");
			mThreadStatus1 = true;
			mLock->unlock();
			continue;
		}

		if (id == eNormal)
		{
			OmnString time = OmnGetTime(AosLocale::eChina);
			job_doc->setAttr(AOSTAG_JOB_CREATED_TIME, time);
			bool rslt = AosModifyDoc(job_doc, rdata);
			if (!rslt)
			{
				OmnAlarm << "modify job doc faild" << enderr;
				AosSetEntityError(rdata, "jobmgr_modify_jobdoc_failed", "JobMgr", "JobMgr");
				mThreadStatus1 = true;
				job->addErrorMsg(job_doc, rdata->getErrmsg(), rdata);
				mLock->unlock();
				continue;
			}
			rdata->setArg1(AOSTAG_JOBSTATUS, "create");
			rdata->setWorkingDoc(job_doc, false);
			AOSSYSLOG_CREATE(true, AOSSYSLOGNAME_JOB, rdata);
		}

		mLock->unlock();
		job->setRundata(rdata);
		OmnTagFuncInfo << "Job mgr start a new job to run" << endl;
		bool rslt = job->startJob(id, job->getRundata());
		if (!rslt)
		{
			OmnAlarm << "runJob error!" << enderr;
			mThreadStatus1 = true;
			continue;
		}

		mLock->lock();
		mStartJobs[job_docid] = job;

		mLock->unlock();
	}
	return true;
}


bool	
AosJobMgr::timerThreadFunc(
		OmnThrdStatus::E &state,
		const OmnThreadPtr &thread)
{
	while (state == OmnThrdStatus::eActive)
	{
		mLock->lock();
		bool rslt = hasSingleStartJob();
		mLock->unlock();

		if (!rslt)
		{
			AosRundataPtr rdata = OmnApp::getRundata();
			rdata = rdata->clone(AosMemoryCheckerArgsBegin);
			rdata->setCid("10000075");
			parseTimerSdoc(rdata);
		}
		
		int i = sgSleepTime;
		while(i--)
		{
			OmnSleep(1);
			mThreadStatus2 = true;
		}
	}
	return true;
}


bool
AosJobMgr::checkStartJobs()
{
	if (mStartJobs.size() <= 0) return true;
	mLock->lock();
	map<u64, AosJobObjPtr> jobs = mStartJobs;
	mLock->unlock();

	map<u64, AosJobObjPtr>::iterator itr = jobs.begin();
	for (;itr != jobs.end(); itr++)
	{
		AosJobObjPtr job = itr->second;
		if (job->isStarted())
		{
			job->checkStartJob();
		}
	}
	return true;
}


bool
AosJobMgr::sendHeartbeat()
{
	if (mStartJobs.size() <= 0) return true;

	bool rslt = false;
	for (int svr_id = 0; svr_id < AosGetNumPhysicals(); svr_id ++)
	{	
		OmnTagFuncInfo << endl;
		rslt = getTaskRunnerInfoBySvrId(OmnApp::getRundata(), svr_id);
		aos_assert_r(rslt, false);
	}
	return true;
}


bool
AosJobMgr::getTaskRunnerInfoBySvrId(
		const AosRundataPtr &rdata,
		const int svr_id)
{
	if (mStartJobs.size() <= 0)  return true;
	u64 job_docid = 0;
	u32 num_slots = 0;
	map<u64, AosJobObjPtr>::iterator itr = mStartJobs.begin();
	while(itr != mStartJobs.end())
	{
		job_docid = itr->first;
		num_slots = (itr->second)->getNumSlots();
		aos_assert_r(job_docid != 0 && num_slots > 0, false);
		AosTransPtr trans = OmnNew AosGetTaskRunnerInfoTrans(
				job_docid, num_slots, AosGetSelfServerId(), svr_id);
		AosSendTransAsyncResp(rdata, trans);
		itr++;
	}
	return true;
}


bool
AosJobMgr::svrDeath(const int death_svr_id)
{
	if(mStartJobs.size() <= 0) return true;
	mLock->lock();
	map<u64, AosJobObjPtr> jobs = mStartJobs;
	mLock->unlock();

	map<u64, AosJobObjPtr>::iterator itr = jobs.begin();
	for (;itr != jobs.end(); itr++)
	{
		AosJobObjPtr job = itr->second;
		if (job->isStarted())
		{
			job->svrDeath(death_svr_id);
		}
	}
	return true;
}


bool
AosJobMgr::heartbeatCallBack(
		const bool is_svr_death,
		const int svr_id)
{
	if (!is_svr_death) 
	{
		setProcStatus(svr_id, eStarted);
		return true;
	}

	setProcStatus(svr_id, eUnusable);
	AosTaskDriver::getSelf()->addSvrDeathEvent(svr_id);
	return true;
}


bool
AosJobMgr::scheduleNextTask(const AosTaskRunnerInfoPtr &runner_info)
{
	if(mStartJobs.size() <= 0) return true;

	u64 job_docid = runner_info->getJobDocid();

	mLock->lock();
	if (mStartJobs.find(job_docid) == mStartJobs.end())
	{
		mLock->unlock();
		return true;
	}
	AosJobObjPtr job = mStartJobs[job_docid];
	mLock->unlock();
	aos_assert_r(job, false);
	return job->scheduleNextTask(runner_info, job->getRundata()); 
}

void
AosJobMgr::setProcStatus(
		const u32 svr_id, 
		const ProcStatus status)
{
	map<u32, ProcStatus>::iterator itr = mProcStatus.find(svr_id);
	if (itr == mProcStatus.end())
	{
		mLock->lock();
		mProcStatus.insert(make_pair(svr_id, status));
		mLock->unlock();
	}
	if (itr->second == status) return;

	mLock->lock();
	mProcStatus[svr_id] = status;
	mLock->unlock();
	return;
}


AosJobMgrObj::ProcStatus
AosJobMgr::getProcStatus(const u32 svr_id)
{
	//u64 key = ((u64)svr_id << 32) + proc_id;
	map<u32, ProcStatus>::iterator itr = mProcStatus.find(svr_id);
	if (itr == mProcStatus.end()) return eStarted;
	return mProcStatus[svr_id];
}


int
AosJobMgr::getNumUnusableProc()
{
	int num_proc = 0;
	map<u32, ProcStatus>::iterator itr = mProcStatus.begin();
	for (; itr != mProcStatus.end(); ++itr)
	{
		if (itr->second == eUnusable) num_proc ++;
	}
	return num_proc;
}


void
AosJobMgr::parseTimerSdoc(const AosRundataPtr &rdata)
{
	AosXmlTagPtr xml = AosGetDocByObjid(sgTimerSdocObjid, rdata);
	if (!xml) return;
				
	u32 sec = OmnGetSecond();
	u64 week = AosTimeUtil::getDayOfWeek(sec);
	u64 hour = AosTimeUtil::getHourOfDay(sec);

	AosXmlTagPtr tag;
	switch(week)
	{
	case 0 : tag = xml->getFirstChild("Sunday");
	case 1 : tag = xml->getFirstChild("Monday");
	case 2 : tag = xml->getFirstChild("Tuesday");
	case 3 : tag = xml->getFirstChild("Wednesday");
	case 4 : tag = xml->getFirstChild("Thursday");
	case 5 : tag = xml->getFirstChild("Friday");
	case 6 : tag = xml->getFirstChild("Saturday");
	default: break;
	}
	if (!tag) return;

	OmnString tagname = "hour_";
	tagname << hour;
	tag = tag->getFirstChild(tagname);
	if (!tag) return;
					
	AosXmlTagPtr entry = tag->getFirstChild(true);
	while(entry)
	{
		OmnString sdoc_objid = entry->getNodeText();
		createJobObj(sdoc_objid, rdata);	
		entry = tag->getNextChild();
	}
	return;
}


bool
AosJobMgr::createJobObj(const OmnString &sdoc_objid, const AosRundataPtr rdata)
{
	aos_assert_r(sdoc_objid != "", false);

	OmnString str;
	str << "<embedobj "
		<<	"zky_jobname=\"merge_iil\" "
		<<	"zky_ctnrobjid=\"task_ctnr.10000075\" "
		<<	"zky_scheduled=\"false\" "
		<<	"zky_singlerun=\"true\" "
		<<	"zky_sdoc_objid=\"" << sdoc_objid << "\" " 
		<<	"zky_status=\"stop\" "
		<<	"zky_jobid=\"rootjob\" "
		<<	"zky_public_doc=\"true\" "
		<<	"zky_public_ctnr=\"true\" "
		<<	"zky_pctrs=\"jobunicom\"/>";

	AosXmlTagPtr xml = AosCreateDoc(str, true, rdata);
	aos_assert_r(xml, false);
	
	OmnString objid = xml->getAttrStr(AOSTAG_OBJID);
	aos_assert_r(objid != "", false);

	OmnString args = "mid=jobmgr,cmdid=startjob,objid=";
	args << objid;

	OmnTagFuncInfo << "create a job object to run a command" << endl;
	runCommand(args, 0, rdata);
	return true;
}


bool
AosJobMgr::hasSingleStartJob()
{
	map<u64, AosJobObjPtr>::iterator itr = mStartJobs.begin();
	for (;itr != mStartJobs.end(); itr++)
	{
		AosJobObjPtr job = itr->second;
		bool status = job->getSingleRunningStatus();
		if (status)
		{
			OmnTagFuncInfo << "yes, has single start job" << endl; 
			return true;
		}
	}
	return false;
}


bool
AosJobMgr::signal(const int threadLogicId)
{
	if (threadLogicId == 0)
	{
		mLock->lock();
		mCondVar->signal();
		mLock->unlock();
	}
	return true;
}


// felicia, 2013/06/18
bool    
AosJobMgr::checkThread111(OmnString &err, const int thrdLogicId)
{
	mLock->lock();
	bool threadStatus;
	switch(thrdLogicId)
	{
		case 0 :
			threadStatus = mThreadStatus1;	
			mThreadStatus1 = false;
			break;
		case 1 :
			threadStatus = mThreadStatus2;
			mThreadStatus2 = false;
			break;
		case 2 :
			threadStatus = mThreadStatus3;
			mThreadStatus3 = false;
			break;
		default:
			OmnAlarm << "Invalid thread Id ! " << enderr;
	}
	mLock->unlock();
	return threadStatus;
}


bool 
AosJobMgr::runCommand(
		const OmnString &args, 
		const AosXmlTagPtr &root, 
		const AosRundataPtr &rdata)
{
	// A front-end user issued a command to run on this module. 
	// 'args' is a string of name-value pairs, separated by ','. 
	// 'root' is the request received from the user.
	OmnString cid = rdata->getCid();
	if (cid == "")
	{
		AosSetErrorU(rdata, "missing_cloudid:");
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	bool rslt = checkDiskFreeSize(rdata);
	if (!rslt)
	{
		AosSetErrorU(rdata, "disk free size too small");
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	OmnTagFuncInfo << "args are: " << args << endl;

	OmnString cmdid, job_objid;
	rslt = AosParseArgs(args, "objid", job_objid, "cmdid", cmdid);
	if (!rslt || cmdid == "")
	{
		AosSetErrorU(rdata, "missing_cmdid:") << args;
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}
	
	if (job_objid == "")
	{
		AosSetErrorU(rdata, "missing_job_objid:") << args;
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}
	
	AosNetCommand::E command = AosNetCommand::toEnum(cmdid);
	switch (command)
	{
	case AosNetCommand::eReStartJob:
		 return reStartJob(args, job_objid, rdata);

	case AosNetCommand::eStartJob:
		 return startJob(args, job_objid, 0, rdata);

	case AosNetCommand::eStopJob:
		 return stopJob(args, job_objid, rdata);

	case AosNetCommand::eSwitchJobMaster:
		 return startJobBySwitchMaster(args, job_objid, rdata);

	default:
		 break;
	}

	AosSetErrorU(rdata, "unrecog_command:") << args;
	OmnAlarm << rdata->getErrmsg() << enderr;
	return false;
}

AosJobObjPtr
AosJobMgr::getStartJobLocked(
		const u64 &job_docid,
		const AosRundataPtr &rdata)
{
	map<u64, AosJobObjPtr>::iterator itr = mStartJobs.find(job_docid);
	if (itr != mStartJobs.end())
	{
		AosJobObjPtr job = itr->second;
		return job;
	}
	OmnScreen << "Job not in Running Map: " << job_docid << endl;
	return 0;
}




AosJobObjPtr
AosJobMgr::getStartJob(
		const u64 &job_docid,
		const AosRundataPtr &rdata)
{
	// This function retrieves the job by 'job_docid'. If the job
	// is not in memory, it does nothing. Otherwise, it returns the job.
	mLock->lock();
	map<u64, AosJobObjPtr>::iterator itr = mStartJobs.find(job_docid);
	if (itr != mStartJobs.end())
	{
		AosJobObjPtr job = itr->second;
		mLock->unlock();
		return job;
	}

	OmnScreen << "Job not in Running Map: " << job_docid << endl;
	mLock->unlock();
	return 0;
}

	
AosJobObjPtr
AosJobMgr::getJobByObjidPriv(
		const OmnString &job_objid,
		const AosRundataPtr &rdata)
{
	AosJobObjPtr job;
	map<u64, AosJobObjPtr>::iterator map_itr = mStartJobs.begin();
	while(map_itr != mStartJobs.end())
	{
		job = map_itr->second;
		if (job->getJobObjid() == job_objid)
		{
			return job;
		}
		map_itr++;
	}
	
	// not find the job.
	OmnScreen << "Job not in memory: " << job_objid << endl;
	return 0;
}

bool
AosJobMgr::removeWaitJob(const u64 &job_docid)
{
	mLock->lock();
	JobInfo info;
	deque<JobInfo>::iterator deque_itr = mWaitJobInfos.begin();
	while(deque_itr != mWaitJobInfos.end())
	{
		info = *deque_itr;
		if (info.mJobDocid == job_docid)
		{
			mLock->unlock();
			mWaitJobInfos.erase(deque_itr);
			return true;
		}
		deque_itr++;
	}
	mLock->unlock();
	return true;
}


bool
AosJobMgr::getWaitJob(
		const u64 &job_docid,
		const AosRundataPtr &rdata)
{
	mLock->lock();
	JobInfo info;
	deque<JobInfo>::iterator deque_itr = mWaitJobInfos.begin();
	while(deque_itr != mWaitJobInfos.end())
	{
		info = *deque_itr;
		if (info.mJobDocid == job_docid)
		{
			mLock->unlock();
			return true;
		}
		deque_itr++;
	}
	mLock->unlock();
	return false;
}
	
AosJobObjPtr
AosJobMgr::getJobByObjid(
		const OmnString &job_objid,
		const AosRundataPtr &rdata)
{
	mLock->lock();
	AosJobObjPtr job = getJobByObjidPriv(job_objid, rdata);
	mLock->unlock();
	return job;
}

	
AosJobObjPtr
AosJobMgr::getJobByDocid(
		const u64 job_docid,
		const AosRundataPtr &rdata)
{
	mLock->lock();
	AosJobObjPtr job = getJobByDocidPriv(job_docid, rdata);
	mLock->unlock();
	return job;
}


AosJobObjPtr
AosJobMgr::getJobByDocidPriv(
		const u64 job_docid,
		const AosRundataPtr &rdata)
{
	AosJobObjPtr job;
	map<u64, AosJobObjPtr>::iterator map_itr = mStartJobs.find(job_docid);
	if (map_itr != mStartJobs.end())
	{
		job = map_itr->second;
		return job;
	}
	
	// not find the task.
	OmnScreen << "Job not in memory: " << job_docid << endl;
	return 0;
}


bool
AosJobMgr::removeStartJob(
		const AosJobObjPtr &job,
		const AosRundataPtr &rdata)
{
	aos_assert_r(job, false);

	u64 job_docid = job->getJobDocid();
	if (!job_docid)
	{
		AosSetErrorU(rdata, "failed_get_job_docid:") << job_docid;
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}
	
	mLock->lock();
	map<u64, AosJobObjPtr>::iterator itr = mStartJobs.find(job_docid);
	if (itr == mStartJobs.end())
	{
		// Did not find the job. 
		mLock->unlock();
		AosSetErrorU(rdata, "job_not_found_in_map:") << job_docid;
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	OmnScreen << "erase job from jobmgr, job_docid:" << job_docid << endl;
	mStartJobs.erase(job_docid);
	mCondVar->signal();
	mLock->unlock();
	return true;
}


AosJobObjPtr
AosJobMgr::createJob(
		const OprId id,
		const AosXmlTagPtr &job_doc, 
		const AosXmlTagPtr &job_sdoc, 
		const AosRundataPtr &rdata)
{
	aos_assert_r(job_doc && job_sdoc, 0);
	OmnTagFuncInfo << endl;
	AosJobObjPtr job = AosJob::staticCreateJob(id, job_doc, job_sdoc, rdata);
	return job;
}

bool
AosJobMgr::addReq(
		const AosRundataPtr &rdata,
		const AosTransPtr &trans)
{
	//Jozhi 2013/03/20
	bool rslt = AosSendTrans(rdata, trans);
	aos_assert_rr(rslt, rdata, false);
	rdata->setOk();
	return true;
}


bool
AosJobMgr::addReq(
		const AosRundataPtr &rdata,
		const AosTransPtr &trans,
		AosBuffPtr &resp)
{
	//Jozhi 2013/03/20
	bool timeout = false;
	bool rslt = AosSendTrans(rdata, trans, timeout, resp);
	aos_assert_r(rslt, false);

	if (timeout)
	{
		AosSetErrorU(rdata, "Faild to add the trans, timeout");
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}
	
	if (!resp|| resp->dataLen() < 0)
	{
		AosSetErrorU(rdata, "no response");
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	return true;
}


//Jozhi 2013-09-23
bool
AosJobMgr::reStartJob(
		const OmnString &args,
		const OmnString &job_objid,
		const AosRundataPtr &rdata)
{
	aos_assert_r(job_objid != "", false);
	AosJobObjPtr job = getJobByObjid(job_objid, rdata);
	if (job)
	{
		if (!job->isStoped())
		{
			AosSetErrorU(rdata, "job  started : ") << job_objid;
			OmnAlarm << rdata->getErrmsg() << enderr;
			return false;
		}
		return true;
	}

	AosXmlTagPtr job_doc = AosGetDocByObjid(job_objid, rdata);
	if(!job_doc)
	{
		AosSetErrorU(rdata, "job doc is not found : ") << job_objid;
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	u64 job_docid = job_doc->getAttrU64(AOSTAG_DOCID, 0);
	aos_assert_r(job_docid != 0, false);

	bool isExist = getWaitJob(job_docid, rdata);
	if (isExist)
	{
		OmnAlarm << "job is waited" << enderr;
		return true;
	}

	OmnString sdoc_objid = job_doc->getAttrStr(AOSTAG_SDOC_OBJID, "");
	if (sdoc_objid == "")
	{
		sdoc_objid = job_objid;
	}
	
	AosXmlTagPtr job_sdoc = AosGetDocByObjid(sdoc_objid, rdata); 
	if (!job_sdoc)
	{
		AosSetErrorU(rdata, "failed get job smartdoc : ") << sdoc_objid;
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	int job_serverid = job_doc->getAttrInt(AOSTAG_JOBSERVERID, -1);
	if(job_serverid >= 0 && job_serverid != AosGetSelfServerId())
	{
		AosTransPtr trans = OmnNew AosSendCmdTrans(args, job_serverid);
		bool rslt = addReq(rdata, trans);
		aos_assert_r(rslt, false);
		return true;
	}

	mLock->lock();
	if (mWaitJobInfos.size() >= eMaxWaitJobs)
	{
		mLock->unlock();
		AosSetErrorU(rdata, "too_many_jobs");
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	job_doc->setAttr(AOSTAG_STATUS, AOSJOBSTATUS_WAIT);
	bool rslt = AosModifyDoc(job_doc, rdata);
	aos_assert_r(rslt, false);

	JobInfo info;
	info.mOpr = eReStart;
	info.mJobDocid = job_docid;
	info.mJobDoc = job_doc;
	info.mJobSdoc = job_sdoc;
	info.mRundata = rdata->clone(AosMemoryCheckerArgsBegin);
	info.mArgs = args;
	mWaitJobInfos.push_back(info);
	mCondVar->signal();
	mLock->unlock();
	return true;
}

bool
AosJobMgr::startJob(
		const OmnString &args,
		const OmnString &job_objid,
		const AosXmlTagPtr &job_doc_conf,
		const AosRundataPtr &rdata)
{
	//Jozhi fix bugs JIMODB-327
	AosXmlTagPtr job_doc = AosGetDocByObjid(job_objid, rdata);
	if (!job_doc)
	{
		OmnString msg;
		msg << "[Err] : The job doc is not exist";
		rdata->setJqlMsg(msg);                                
		return false;                                           
	}
	aos_assert_r(job_objid != "", false);
	AosJobObjPtr job = getJobByObjid(job_objid, rdata);
	if (job)
	{
		if (!job->isStoped())
		{
			AosSetErrorU(rdata, "job  started : ") << job_objid;
			OmnAlarm << rdata->getErrmsg() << enderr;
			return false;
		}
		return true;
	}

	//Gavin 2015/07/26
	aos_assert_r(job_doc, false);
	OmnString str = job_doc->getAttrStr("zky_status");           
	if (str == "start" || str == "finish")                      
	{                                                           
		OmnString msg;                                        
		msg << "[ERR] : The job has been finished or being running!";
		rdata->setJqlMsg(msg);                                
		return false;                                           
	}                                                           

	//young.pan 20l4/7/9
	AosXmlTagPtr job_sdoc;
	if (job_doc_conf)
	{
		job_doc = job_doc_conf;
		job_sdoc = job_doc_conf;
	}
	else
	{
		job_doc = AosGetDocByObjid(job_objid, rdata);
	}

	if(!job_doc)
	{
		OmnString msg = "job is not found!";
		rdata->setJqlMsg(msg);
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	u64 job_docid = job_doc->getAttrU64(AOSTAG_DOCID, 0);
	aos_assert_r(job_docid != 0, false);

	bool isExist = getWaitJob(job_docid, rdata);
	if (isExist)
	{
		OmnAlarm << "job is waited" << enderr;
		return true;
	}

	OmnString sdoc_objid; 
	if (!job_doc_conf)
	{
		sdoc_objid = job_doc->getAttrStr(AOSTAG_SDOC_OBJID, "");
		if (sdoc_objid == "")
		{
			AosSetEntityError(rdata, "jobmgr_can_not_get_job_smartdoc_objid", "JobMgr", "JobMgr")
				<< sdoc_objid << enderr;
			return false;
		}
		job_sdoc = AosGetDocByObjid(sdoc_objid, rdata); 
	}

	if (!job_sdoc)
	{
		AosSetErrorU(rdata, "failed get job smartdoc : ") << sdoc_objid;
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	OmnString job_status = job_doc->getAttrStr(AOSTAG_STATUS, AOSJOBSTATUS_STOP);
	if (job_status == AOSJOBSTATUS_START || job_status == AOSJOBSTATUS_WAIT)
	{
		AosSetEntityError(rdata, "jobmgr_job_has_started", "JobMgr", "JobMgr");
		return false;
	}

	if (job_status == AOSJOBSTATUS_FINISH)
	{
		rdata->setOk();
		return true;
	}

	int job_serverid = job_doc->getAttrInt(AOSTAG_JOBSERVERID, -1);
	if(job_serverid >= 0 && job_serverid != AosGetSelfServerId())
	{
		AosTransPtr trans = OmnNew AosSendCmdTrans(args, job_serverid);
		bool rslt = addReq(rdata, trans);
		aos_assert_r(rslt, false);
		return true;
	}

	mLock->lock();
	if (mWaitJobInfos.size() >= eMaxWaitJobs)
	{
		mLock->unlock();
		AosSetErrorU(rdata, "too_many_jobs");
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	job_doc->setAttr(AOSTAG_JOBID, "rootjob");
	job_doc->setAttr(AOSTAG_STATUS, AOSJOBSTATUS_WAIT);
	bool rslt = AosModifyDoc(job_doc, rdata);
	aos_assert_r(rslt, false);

	JobInfo info;
	info.mOpr = eNormal;
	info.mJobDocid = job_docid;
	info.mJobDoc = job_doc;
	info.mJobSdoc = job_sdoc;
	info.mRundata = rdata->clone(AosMemoryCheckerArgsBegin);
	info.mArgs = args;

	mWaitJobInfos.push_back(info);
	mCondVar->signal();
	mLock->unlock();
	return true;
}


bool
AosJobMgr::stopJob(
		const OmnString &args, 
		const OmnString &job_objid,
		const AosRundataPtr &rdata)
{
 	OmnTagFuncInfo << "args are: " << args << endl;
	AosJobObjPtr job = getJobByObjid(job_objid, rdata);
	bool rslt = false;
	if (!job)
	{
		AosXmlTagPtr job_doc = AosGetDocByObjid(job_objid, rdata);
		aos_assert_r(job_doc, false);
		u64 job_docid = job_doc->getAttrU64(AOSTAG_DOCID, 0);
		aos_assert_r(job_docid != 0 , false);
		OmnString status_str = job_doc->getAttrStr(AOSTAG_STATUS, AOSJOBSTATUS_STOP);
		if (status_str == AOSJOBSTATUS_STOP)
		{
			OmnScreen << "job is stopping, stop it now: " << job_docid << endl;
			return true;
		}
		job_doc->setAttr(AOSTAG_STATUS, AOSJOBSTATUS_STOP);
		rslt = AosModifyDoc(job_doc, rdata);
		aos_assert_r(rslt, false);
		removeWaitJob(job_docid);
		OmnScreen << "job is waiting , stop it now:" << job_docid << endl;
		return true;
	}

	if (job->isWaited())
	{
		rslt = AosTaskDriver::getSelf()->stopJob(job->getJobDocid(), rdata);
		if (!rslt)
		{
			AosSetErrorU(rdata, "job stop failed");
			OmnAlarm << rdata->getErrmsg() << enderr;
			return false;
		}
	}
	else if (job->isStarted())
	{
		rslt = AosTaskDriver::getSelf()->stopJob(job->getJobDocid(), rdata);
		if (!rslt)
		{
			AosSetErrorU(rdata, "job stop failed");
			OmnAlarm << rdata->getErrmsg() << enderr;
			return false;
		}
	}
	rdata->setOk();
	return true;
}


bool
AosJobMgr::jobStoped(
		const AosJobObjPtr &job,
		const AosRundataPtr &rdata)
{
	OmnScreen << "job stop job_docid: " << job->getJobDocid() << endl;
	return removeStartJob(job, rdata);
}


bool
AosJobMgr::jobFailed(
		const AosJobObjPtr &job,
		const AosRundataPtr &rdata)
{
	return removeStartJob(job, rdata);
}


bool
AosJobMgr::jobFinished(
		const AosJobObjPtr &job,
		const AosRundataPtr &rdata)
{
	return removeStartJob(job, rdata);
}


bool
AosJobMgr::tryStartJob()
{
return true;
	AosRundataPtr rdata = OmnApp::getRundata();
	rdata->setUserid(AosObjMgrObj::getObjMgr()->getSuperUserDocid());
	rdata->setCid(AosObjMgrObj::getObjMgr()->getSuperUserCid());
	rdata->setSiteid(100);
OmnScreen << "jozhi tryStartJob: cid, " << rdata->getCid() << endl;
	vector<OmnString> key_names;
	key_names.push_back("zky_jobid");
	key_names.push_back(AOSTAG_STATUS);
	 
	vector<OmnString> key_values;
	key_values.push_back("rootjob");
	key_values.push_back("start");
	AosXmlTagPtr doc = AosDocClientObj::getDocClient()->getDocByKeys(rdata, "", key_names, key_values);
	if (!doc)
	{
		OmnScreen << "call switchJob method!!!!" << endl;
		return true;
	}
OmnScreen << "switchJob; find Unfinished job_docid:" << doc->getAttrU64(AOSTAG_DOCID, 0) << endl;

	OmnString objid = doc->getAttrStr(AOSTAG_OBJID, "");
	//Jozhi 2015-03-23 
	//may be not known, it's "switch job" or "restart job"
	//OmnString args;
	//if (AosGetNumPhysicals() > 1)
	//{
	//	args << "mid=jobmgr,cmdid=switchjobmaster,objid=";
	//}
	//else
	//{
	//	args << "mid=jobmgr,cmdid=restartjob,objid=";
	//}
	OmnString args;
	args << "mid=jobmgr,cmdid=restartjob,objid=";
	args << objid;

	OmnTagFuncInfo << "try to start running a command" << endl;
	runCommand(args, 0, rdata);
	return true;
}


bool
AosJobMgr::startJobBySwitchMaster(
		const OmnString &args,
		const OmnString &job_objid,
		const AosRundataPtr &rdata)
{
	aos_assert_r(job_objid != "", false);
	AosJobObjPtr job = getJobByObjid(job_objid, rdata);
	if (job)
	{
		if (!job->isStoped())
		{
			AosSetErrorU(rdata, "job  started : ") << job_objid;
			OmnAlarm << rdata->getErrmsg() << enderr;
			return false;
		}
		return true;
	}

	AosXmlTagPtr job_doc = AosGetDocByObjid(job_objid, rdata);
	if(!job_doc)
	{
		AosSetErrorU(rdata, "job doc is not found : ") << job_objid;
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}
OmnScreen << "switch Job AA;" << job_doc->toString() << endl;

	u64 job_docid = job_doc->getAttrU64(AOSTAG_DOCID, 0);
	aos_assert_r(job_docid != 0, false);

	bool isExist = getWaitJob(job_docid, rdata);
	if (isExist)
	{
		OmnAlarm << "job is waited" << enderr;
		return true;
	}

	OmnString sdoc_objid = job_doc->getAttrStr(AOSTAG_SDOC_OBJID, "");
	if (sdoc_objid == "")
	{
		sdoc_objid = job_objid;
	}
	
	AosXmlTagPtr job_sdoc = AosGetDocByObjid(sdoc_objid, rdata); 
	if (!job_sdoc)
	{
		AosSetErrorU(rdata, "failed get job smartdoc : ") << sdoc_objid;
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	mLock->lock();
	if (mWaitJobInfos.size() >= eMaxWaitJobs)
	{
		mLock->unlock();
		AosSetErrorU(rdata, "too_many_jobs");
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	JobInfo info;
	info.mOpr = eSwitchJobMaster;
	info.mJobDocid = job_docid;
	info.mJobDoc = job_doc;
	info.mJobSdoc = job_sdoc;
	info.mRundata = rdata->clone(AosMemoryCheckerArgsBegin);
	info.mArgs = args;
	mWaitJobInfos.push_back(info);
	mCondVar->signal();
	mLock->unlock();
	return true;
}


bool
AosJobMgr::checkDiskFreeSize(const AosRundataPtr &rdata)
{
	return true;
	int num_phy = AosGetNumPhysicals();

	bool rslt;
	AosTransPtr trans;
	AosBuffPtr resp;
	for (int i=0; i<num_phy; i++)
	{
		trans = OmnNew AosCheckDiskFreeSizeTrans(i);
		resp = 0;
		rslt = addReq(rdata, trans, resp);
		aos_assert_r(rslt && resp, false);
		
		rslt = resp->getU8(0);
		if (!rslt)
		{
			OmnAlarm << "check disk free size failed, physicalid:" << i << enderr;
			return false;
		}
	}

	return true;
}


bool
AosJobMgr::checkDiskFreeSizeLocal(const AosRundataPtr &rdata)
{
	vector<AosDiskInfo> dsInfos;
	bool rslt = AosGetDiskInfo(dsInfos);
	aos_assert_r(rslt, false);
	
	bool found;
	set<OmnString>::iterator itr = mDiskNameSet.begin();	
	while (itr != mDiskNameSet.end())
	{
		found = false;
		for (u32 i=0; i<dsInfos.size(); i++)
		{
			if ((*itr) == dsInfos[i].mMountedDir)
			{
				found = true;
				if (dsInfos[i].mFree < mSafeDiskFreeSize)
				{
					OmnAlarm << "disk free size is not enough:" << (*itr)
						<< ", now free:" << dsInfos[i].mFree
						<< ", need free:" << mSafeDiskFreeSize << enderr;
					return false;
				}
				break;
			}
		}

		if (!found)
		{
			OmnAlarm << "missing disk:" << (*itr) << enderr;
			return false;
		}

		itr++;
	}

	return true;
}

