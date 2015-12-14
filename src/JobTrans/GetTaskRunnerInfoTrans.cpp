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
// 07/28/2011	Created by Linda 
////////////////////////////////////////////////////////////////////////////
#include "JobTrans/GetTaskRunnerInfoTrans.h"

#include "API/AosApi.h"
#include "XmlUtil/XmlTag.h"
#include "SEInterfaces/JobMgrObj.h"
#include "Job/JobMgr.h"
#include "TaskUtil/TaskDriver.h"
#include "TaskUtil/TaskRunnerInfo.h"
#include "TaskUtil/TaskDriverRequest.h"
#include "SEInterfaces/TaskMgrObj.h"


AosGetTaskRunnerInfoTrans::AosGetTaskRunnerInfoTrans(const bool regflag)
:
AosTaskTrans(AosTransType::eGetTaskRunnerInfo, regflag)
{
}


AosGetTaskRunnerInfoTrans::AosGetTaskRunnerInfoTrans(
		const u64 &job_docid,
		const u32 num_slots,
		const int crt_job_svrid,
		const int server_id)
:
AosTaskTrans(AosTransType::eGetTaskRunnerInfo, server_id, false, true),
mJobDocid(job_docid),
mNumSlots(num_slots),
mCrtJobSvrId(crt_job_svrid),
mTaskSvrId(server_id)
{
//	OmnScreen << "AosGetTaskRunnerInfoTrans AosGetTaskRunnerInfoTrans mTaskSvrId:" << mTaskSvrId << endl;
}


AosGetTaskRunnerInfoTrans::~AosGetTaskRunnerInfoTrans()
{
}


bool
AosGetTaskRunnerInfoTrans::serializeFrom(const AosBuffPtr &buff)
{
	bool rslt = AosTaskTrans::serializeFrom(buff);
	aos_assert_r(rslt, false);

	mJobDocid = buff->getU64(0);
	aos_assert_r(mJobDocid != 0, false);
	mNumSlots = buff->getU32(0);
	aos_assert_r(mNumSlots > 0, false);
	mCrtJobSvrId = buff->getInt(0);
	mTaskSvrId = buff->getInt(0);
	return true;
}


bool
AosGetTaskRunnerInfoTrans::serializeTo(const AosBuffPtr &buff)
{
	bool rslt = AosTaskTrans::serializeTo(buff);
	aos_assert_r(rslt, false);

	aos_assert_r(mJobDocid != 0, false);
	buff->setU64(mJobDocid);
	aos_assert_r(mNumSlots > 0, false);
	buff->setU32(mNumSlots);
	buff->setInt(mCrtJobSvrId);
	buff->setInt(mTaskSvrId);
	return true;
}


AosTransPtr
AosGetTaskRunnerInfoTrans::clone()
{
	return OmnNew AosGetTaskRunnerInfoTrans(false);
}


bool
AosGetTaskRunnerInfoTrans::proc()
{
	OmnScreen << "proc getTaskProcess"
		<< "; trans_id:" << getTransId().toString() << endl;
	aos_assert_r(AosGetSelfServerId() == mTaskSvrId, false);

	vector<AosTaskRunnerInfoPtr> runner_infos;
	bool rslt = AosTaskMgrObj::getTaskRunnerInfosStatic(mJobDocid, mNumSlots, mCrtJobSvrId, runner_infos, mRdata);
	aos_assert_r(rslt, false);
	
	int size = runner_infos.size();
	aos_assert_r(size > 0, false);

	AosBuffPtr resp_buff = OmnNew AosBuff(AosMemoryCheckerArgsBegin);
	resp_buff->setInt(size);

	AosTaskRunnerInfoPtr runner_info;
	vector<AosTaskRunnerInfoPtr>::iterator itr = runner_infos.begin();
//OmnScreen << "jozhi for debug start=====================trans_id: " << getTransId().toString() << endl;
	while (itr != runner_infos.end())
	{
		runner_info = *itr;
//OmnScreen << "===jozhi for debug GetTaskRunnerInfos task_docid: " << runner_info->getTaskDocid() 
//				<< " , status: " << AosTaskStatus::toStr(runner_info->getStatus()) 
//				<< ", trans_id: " << getTransId().toString() << endl;
		rslt = runner_info->serializeTo(resp_buff);
		aos_assert_r(rslt, false);
		itr++;
	}
//OmnScreen << "jozhi for debug end=====================trans_id: " << getTransId().toString() << endl;

	sendResp(resp_buff);
	mRdata->setOk();
	return true;
}


bool
AosGetTaskRunnerInfoTrans::respCallBack()
{
	aos_assert_r(AosGetSelfServerId() == mCrtJobSvrId, false);
	AosJobMgrObj::getJobMgr()->heartbeatCallBack(isSvrDeath(), mTaskSvrId);
	if (isSvrDeath())
	{
		mRdata->setOk();
		return true;
	}

	AosBuffPtr resp = getResp();
	
	int size = resp->getInt(0);
	aos_assert_r(size > 0, false);

	bool rslt = true;
	AosTaskRunnerInfoPtr runner_info;
	AosTaskDriver::EventTrigger event;
	AosTaskDriverRequestPtr req;
//OmnScreen << "jozhi for debug start=====================trans_id: " << getTransId().toString() << endl;
	for (int i=0; i<size; i++)
	{
		runner_info = OmnNew AosTaskRunnerInfo();
		rslt = runner_info->serializeFrom(resp);
		aos_assert_r(rslt, false);

		if (runner_info->isIdle())
		{
			aos_assert_r(runner_info->getTaskDocid() == 0, false);
			event = AosTaskDriver::eScheduleNextTask;
		}
		else
		{
//OmnScreen << "===jozhi for debug GetTaskRunnerInfos task_docid: " << runner_info->getTaskDocid() 
//				<< " , status: " << AosTaskStatus::toStr(runner_info->getStatus()) 
//				<< ", trans_id: " << getTransId().toString() << endl;
			switch(runner_info->getStatus())
			{
			case AosTaskStatus::eStop:
				 continue;

			case AosTaskStatus::eStart:
				 event = AosTaskDriver::eUpdateTaskProgress;
				 break;

			case AosTaskStatus::eFinish:
				 event = AosTaskDriver::eUpdateTaskStatus;
				 break;

			case AosTaskStatus::eFail:
				 event = AosTaskDriver::eUpdateTaskStatus;
				 break;
		
			default:
				 OmnAlarm << "AAAAAAAAAAAAAA" << enderr;
				 break;
			}
		}

		req = OmnNew AosTaskDriverRequest(event, runner_info, mRdata);
		rslt = AosTaskDriver::getSelf()->addEvent(req);
		if (!rslt)
		{
			mRdata->setError();
			return false;
		}
	}
//OmnScreen << "jozhi for debug end=====================trans_id: " << getTransId().toString() << endl;

	mRdata->setOk();
	return true;
}


