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
// 	Created: 05/09/2010 by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "TaskRunnerMgr/TaskRunnerWrap.h"
#include "TaskRunnerMgr/TaskRunnerMgr.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Debug/Debug.h"
#include "Porting/Sleep.h"
#include "Rundata/Rundata.h"
#include "Util/OmnNew.h"
#include "Util/Buff.h"
#include "XmlUtil/SeXmlParser.h"
#include "XmlUtil/XmlTag.h"
#include "TaskMgr/Task.h"

AosTaskRunnerWrap::AosTaskRunnerWrap(pid_t &pid, int &fd0, int &fd1)
:
mStatus(eIdle),
mHeartBeatTime(OmnGetSecond()),
mTaskProcTime(OmnGetSecond())
{
	mPid = pid;
	mReadFd = fd0;
	mWriteFd = fd1;
	mTask = 0;
}

int 
AosTaskRunnerWrap::readMsg()
{
	int num;
	int rslt = read(mReadFd, (char*)&num, 4);
	AosBuffPtr buff = OmnNew AosBuff(num AosMemoryCheckerArgs);
	rslt = read(mReadFd, buff->data(), num);
OmnScreen <<  "pid : " << getpid() << ", fd :" << mReadFd << ", read msg : " << buff->data() << ", rslt : " << rslt << endl;
	AosXmlParser parser;
	AosXmlTagPtr content = parser.parse(buff->data(), num, "" AosMemoryCheckerArgs);
	if(!content)
	{
		return 0;
	}
	mHeartBeatTime = OmnGetSecond(); 
	OmnString type = content->getAttrStr("type", "");
	if(type == "")
	{
		return 0;
	}
	if(type != "heartbeat")
	{
		//get the status of the taskrunner
		mTaskProcTime = OmnGetSecond(); 
		OmnString status = content->getAttrStr("status", "");
		if(status == "Failed")
		{
			mStatus = eFailed; 
		}
		else if(status == "Success")
		{
			mStatus = eSuccess; 
		}
		else
		{
			OmnAlarm << " **********************+++++++++++++++=============== status is null,  it is not posible!!!!!!  " << enderr;
			mStatus = eAborted; 
		}
		cleanResource();
//OmnScreen << "process : " << process << endl;
	}
	return rslt;
}

bool
AosTaskRunnerWrap::cleanResource()
{
	mTask = 0;
	return true;
}

bool
AosTaskRunnerWrap::taskRunnerFailed()
{
	return true;
}

bool
AosTaskRunnerWrap::addTask(const AosBuffPtr &task)
{
	mStatus = eRunning;
	aos_assert_r(task, false);
	mTask = task;
//	AosBuffPtr buff = OmnNew AosBuff(1000 AosMemoryCheckerArgs);
//	mTask->serializeTo(buff, NULL);
OmnString s;
s << "add task by wrap! fd is " << mWriteFd;
	int len = task->dataLen();
	int rslt = write(mWriteFd, &len, 4);
	rslt = write(mWriteFd, task->data(), len);
//	len = s.length();
//	rslt = write(mWriteFd, s.data(), len);
OmnScreen <<  "pid : " << mPid << ", fd :" << mWriteFd << ", write content  :" << task->data() << ", rslt : " << rslt << ", length : " << len <<  endl;
	return true;
}

int 
AosTaskRunnerWrap::getReadFd()
{
	return mReadFd;
}

bool
AosTaskRunnerWrap::checkValid()
{
	u32 now = OmnGetSecond();
	if(now - mHeartBeatTime > 160)
	{
OmnScreen << "check invalid time :  " << now - mHeartBeatTime << endl;
		return false;
	}
OmnScreen << "check valid time :  " << now - mHeartBeatTime << endl;
	return true;
}

AosBuffPtr
AosTaskRunnerWrap::getTask() const 
{
	return mTask;
}

bool
AosTaskRunnerWrap::kill()
{
	OmnString cmd = "kill ";
	cmd << mPid;
	int i = system(cmd.data());
	if(i==-1)
	{
		return false;
	}
	return true;
}
