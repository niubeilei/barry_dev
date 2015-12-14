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
#include "TaskRunnerMgr/ServerRunner.h"
#include "TaskRunnerMgr/TRMAIN.h"

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
#include <signal.h>

extern int needStop;
OmnSingletonImpl(AosServerRunnerSingleton, AosServerRunner, AosServerRunnerSelf, "AosServerRunner");

AosServerRunner::AosServerRunner()
{
}


AosServerRunner::~AosServerRunner()
{
	mReadThread->stop();
	close(mReadFd);
	close(mWriteFd);
}

bool
AosServerRunner::readMsg()
{
	fd_set rfd;
	FD_ZERO(&rfd);
	FD_SET(mReadFd, &rfd);
	while(1)
	{
		fd_set fd = rfd;
		struct timeval tv = {eHeartBeatTime, 0};
		int rslt = select(1024, &fd, 0, NULL, &tv);
		if (rslt <=0)
		{
			if (rslt==0)
			{
			}
			continue;
		}

		int num;
		rslt = read(mReadFd, (char*)&num, 4);
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


		OmnString s(buff->data(), rslt);
		AosXmlParser parser;
		AosXmlTagPtr root = parser.parse(s, "" AosMemoryCheckerArgs);
		if(!root)
		{
			OmnAlarm << "the message is not the xml!" << enderr;
		}
		else
		{
			bool flag = procMsg(root);
			if(!flag)
			{
				OmnAlarm << "Failed to process the message!" << enderr;
			}
		}
	}
	return true;
}

bool
AosServerRunner::stopServer(const int &value)
{
	if (value == SIGALRM) needStop = true;
	if (value == 14) needStop = true;
	return true;
}

bool
AosServerRunner::procMsg(const AosXmlTagPtr &msg)
{
	AosXmlTagPtr cmd = msg->getFirstChild("cmd");
	aos_assert_r(cmd, false);
	OmnString type = cmd->getAttrStr("type", "");
	aos_assert_r(type != "", false);
	char c = type.data()[0];
	switch(c)
	{
		case 's' :
			if(type == "stop")
			{
				int value  = cmd->getAttrInt("signal", 0);
				stopServer(value);
			}
			break;
		default :
			OmnAlarm << "there is no this command!" << enderr;
			return false;
	}
	return true;
}

bool
AosServerRunner::sendTask(const AosBuffPtr& buff)
{
	return sendMsg(buff);
}

bool
AosServerRunner::sendMsg(const OmnString &content)
{
	AosBuffPtr buff = OmnNew AosBuff(1000 AosMemoryCheckerArgs); 
	buff->setOmnStr(content);
	sendMsg(buff);
	return true;
}

bool
AosServerRunner::sendMsg(const AosBuffPtr &buff)
{
	int len = buff->dataLen();
	int rslt = write(mWriteFd, &len, 4);
	aos_assert_r(rslt == 4, false);
	rslt = write(mWriteFd, buff->data(), len);
	aos_assert_r(rslt == len, false);
	return true;
}

bool 
AosServerRunner::start()
{
cout << "===============****** start the server ********=============" << endl;
    OmnThreadedObjPtr thisPtr(this, false); 
	if(!mReadThread)
	{
		mReadThread = OmnNew OmnThread(thisPtr, "ServerRunnerThread", eReadThrd, true, true);
	}
	mReadThread->start();
	return true;
}

bool 
AosServerRunner::stop()
{
	return true;
}

bool 
AosServerRunner::config(const AosXmlTagPtr &config)
{
	return true;
}

bool
AosServerRunner::signal(const int threadLogicId)
{
	return true;
}

bool    
AosServerRunner::checkThread(OmnString &err, const int thrdLogicId) const
{
	return mThreadStatus;
}

bool
AosServerRunner::threadFunc(OmnThrdStatus::E &state, 
							  const OmnThreadPtr &thread)
{
	if (thread->getLogicId() == eReadThrd)	
	{
		readMsg();
	}
	state = OmnThrdStatus::eIdle;
	return true;
}

bool 
AosServerRunner::setPidAndPipe(pid_t &pid, int &fd0, int &fd1)
{
	mPid = pid;
	mReadFd = fd0;
	mWriteFd = fd1;
	return true;
}
