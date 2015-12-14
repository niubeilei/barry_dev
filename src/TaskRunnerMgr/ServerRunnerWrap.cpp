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
#include "TaskRunnerMgr/ServerRunnerWrap.h"
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

AosServerRunnerWrap::AosServerRunnerWrap(pid_t &pid, int &fd0, int &fd1)
{
	mPid = pid;
	mReadFd = fd0;
	mWriteFd = fd1;
}

bool
AosServerRunnerWrap::reset()
{
	mPid = 0;
	mReadFd = 0;
	mWriteFd = 0;
	return true;
}

AosBuffPtr 
AosServerRunnerWrap::readTask()
{
	int num;
	int rslt = read(mReadFd, (char*)&num, 4);
	AosBuffPtr buff = OmnNew AosBuff(num AosMemoryCheckerArgs);
	rslt = read(mReadFd, buff->data(), num);
	buff->setDataLen(rslt);
OmnScreen <<  "pid : " << getpid() << ", fd :" << mReadFd << ", read msg : " << buff->data() << ", rslt : " << rslt << endl;
	return buff;
}

bool
AosServerRunnerWrap::stopServer(const int& value)
{
	OmnString s;
	s << "<request><cmd type=\"stop\" signal=\"" << value << "\" /></request>";
	bool rslt = sendMsg(s);
	return rslt;
}

bool
AosServerRunnerWrap::sendMsg(const OmnString &content)
{
	int len = content.length();
	int rslt = write(mWriteFd, &len, 4);
	aos_assert_r(rslt == 4, false);
	rslt = write(mWriteFd, content.data(), len);
	aos_assert_r(rslt == len, false);
	return true;
}

int 
AosServerRunnerWrap::getReadFd()
{
	return mReadFd;
}


pid_t
AosServerRunnerWrap::getPid()
{
	return mPid;
}
