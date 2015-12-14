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
#include "TaskRunnerMgr/TaskRunner.h"

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
#include "SingletonClass/SingletonImpl.cpp"

OmnSingletonImpl(AosTaskRunnerSingleton, AosTaskRunner, AosTaskRunnerSelf, "AosTaskRunner");

static OmnString sgHeartbeat = "<heartbeat type=\"heartbeat\"/>";

AosTaskRunner::AosTaskRunner()
:mTaskRunning(false)
{
}


AosTaskRunner::~AosTaskRunner()
{
	mReadThread->stop();
	close(mReadFd);
	close(mWriteFd);
}

bool
AosTaskRunner::readMsg()
{

cout << "$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$   init taskrunner read thread $$$$$$$$$$$$$$$$$$$$$$$$$$$$" << endl;
//OmnScreen << "$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$   init taskrunner read thread $$$$$$$$$$$$$$$$$$$$$$$$$$$$" << endl;
	fd_set rfd;
	FD_ZERO(&rfd);
	FD_SET(mReadFd, &rfd);

//int testnum = 0;
	while(1)
	{
		fd_set fd = rfd;
		struct timeval tv = {eHeartBeatTime, 0};
		int rslt = select(1024, &fd, 0, NULL, &tv);
		if (rslt <=0)
		{
			if (rslt==0)
			{
//testnum++;
//if(testnum==1)
//{
				//time out
//OmnScreen << "read time out for  " << (int)eHeartBeatTime << endl;
//close the heartbeat
	//			sendMsg(sgHeartbeat);
//}
			}
			continue;
		}
//OmnScreen << "readding task ++++++++++++" << endl;

		int num;
		rslt = read(mReadFd, (char*)&num, 4);
//OmnScreen << "read num is " << num << endl;
		if(rslt != 4)
		{
			OmnAlarm << "read num is not 4" << enderr;
			return false;
		}
		AosBuffPtr buff = OmnNew AosBuff(num AosMemoryCheckerArgs);
		rslt = read(mReadFd, buff->data(), num);
		if(rslt != num)
		{
			OmnAlarm << "read num is not equel to rslt : " << rslt << " num : " << num << enderr;
			return false;
		}
		buff->setDataLen(rslt);
//OmnScreen <<  "pid : " << getpid() << ", fd :" << mReadFd << ", read msg : " << buff->data() << ", rslt : " << rslt << endl;

		bool runflag = procTask(buff);
		if(!runflag)
		{
			OmnString status = "Failed";
			taskFinished(status);
		}
	}
	return true;
}

bool
AosTaskRunner::taskFinished(const OmnString &status)
{
	mTaskRunning = false;
	OmnString  content = "<task type=\"report\" task_id =\"100\" status=\"";
	if(status != "")
	{
		content << status << "\"/>";
	}
	else
	{
		content << "Success\"/>";
	}
	bool rslt = sendMsg(content);
	return rslt;
}

bool
AosTaskRunner::sendMsg(const OmnString &content)
{
	int len = content.length();
	int rslt = write(mWriteFd, &len, 4);
	aos_assert_r(rslt == 4, false);
	rslt = write(mWriteFd, content.data(), len);
	aos_assert_r(rslt == len, false);
//OmnScreen <<  "pid : " << getpid() << ", fd :" << mWriteFd << ", write content  :" << content.data() << ", rslt : " << rslt << ", length : " << len <<  endl;
	return true;
}


bool
AosTaskRunner::procTask(const AosBuffPtr &buff)
{
	AosRundataPtr rdata = OmnNew AosRundata(AosMemoryCheckerArgsBegin);
	u32 sid = 100;
	rdata->setSiteid(sid);
	AosTaskPtr task = AosTask::serializeFromStatic(buff, rdata);
	aos_assert_r(task, false);
	task->setRundata(rdata);
	mTaskRunning = true;
	try
	{
		AosTaskThrdPtr task_thread = OmnNew AosTaskThrd(task, rdata);
//OmnScreen <<  "pid : " << getpid() << ", fd :" << mWriteFd << ", proc task  :" << buff  <<  endl;
		task->setTaskThread(task_thread);
	}
	
	catch (...)
	{
		OmnAlarm << "************** task fail to run ************" << enderr;
		task->taskFailed(rdata);
		return false;
	}
	return true;


}

bool
AosTaskRunner::sendMsg(const AosXmlTagPtr &xml)
{
	OmnString str = xml->toString(); 
	sendMsg(str);
	return true;
}

bool
AosTaskRunner::runTask(const AosTaskPtr task)
{
	//AosXmlTagPtr content;


	bool rslt;
	for (int i=0; i<5; i++)
	{
		OmnString  content = "<task type=\"report\" task_id =\"100\" process=\"";
		content << 20*i << "\"/>";
		rslt = sendMsg(content);
		OmnSleep(10);
	}
	return rslt;
}

bool 
AosTaskRunner::start()
{
cout << "===============****** start the taskrunner********=============" << endl;
//OmnScreen << "===============****** start the taskrunner********=============" << endl;
    OmnThreadedObjPtr thisPtr(this, false); 
	if(!mReadThread)
	{
		mReadThread = OmnNew OmnThread(thisPtr, "TaskRunnerThread", eReadThrd, true, true);
	}
	mReadThread->start();
	if(!mRunTaskThread)
	{
		mRunTaskThread = OmnNew OmnThread(thisPtr, "TaskRunnerThread", eRunTaskThrd, true, true);
	}
	mRunTaskThread->start();
	return true;
}

bool 
AosTaskRunner::stop()
{
	return true;
}

bool 
AosTaskRunner::config(const AosXmlTagPtr &config)
{
	return true;
}

bool
AosTaskRunner::signal(const int threadLogicId)
{
	return true;
}

bool    
AosTaskRunner::checkThread(OmnString &err, const int thrdLogicId) const
{
	return mThreadStatus;
}

bool
AosTaskRunner::threadFunc(OmnThrdStatus::E &state, 
							  const OmnThreadPtr &thread)
{
	if (thread->getLogicId() == eReadThrd)	
	{
		readMsg();
	}
	if (thread->getLogicId() == eRunTaskThrd)
	{
		//runTask(mTask);
	}
	state = OmnThrdStatus::eIdle;
	return true;
}

bool 
AosTaskRunner::setPidAndPipe(pid_t &pid, int &fd0, int &fd1)
{
	mPid = pid;
	mReadFd = fd0;
	mWriteFd = fd1;
	return true;
}
