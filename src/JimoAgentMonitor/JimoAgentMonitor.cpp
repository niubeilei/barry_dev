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
// 	Created: 05/09/2010 by jozhi Peng
////////////////////////////////////////////////////////////////////////////
#include "JimoAgentMonitor/JimoAgentMonitor.h"
#include "JimoAgentMonitor/RecveJimoAgentProc.h"
#include "JimoAgentMonitor/Ptrs.h"
#include "SingletonClass/SingletonImpl.cpp"
#include "Thread/Mutex.h"
#include "Thread/Thread.h"
#include "Porting/Sleep.h"

OmnSingletonImpl(AosJimoAgentMonitorSingleton,
                 AosJimoAgentMonitor,
                 AosJimoAgentMonitorSelf,
                "AosJimoAgentMonitor");


AosJimoAgentMonitor::AosJimoAgentMonitor()
:
mLock(OmnNew OmnMutex()),
mReqDistr(OmnNew AosReqDistr(OmnNew AosRecveJimoAgentProc()))
{
}


AosJimoAgentMonitor::~AosJimoAgentMonitor()
{
}

bool
AosJimoAgentMonitor::threadFunc(
		OmnThrdStatus::E &state,
		const OmnThreadPtr &thread)
{
	while (state == OmnThrdStatus::eActive)
	{
		u64 crt_time = OmnGetSecond();
		mLock->lock();
		map<OmnString, StatusInfo>::iterator itr = mJimoAgentMap.begin();
		OmnScreen << "jozhi: " << "start to monitor" << endl;
		OmnString str;
		str << "\n+---------------+---------------+\n"
			<< "|ip(" << mJimoAgentMap.size() << ")\t\t|status\t\t|\n"
			<< "+---------------+---------------+\n";
		while(itr != mJimoAgentMap.end())
		{
			u64 heartbeat_time = (itr->second).mUpdateTime;
			if ((crt_time - heartbeat_time) > eTimeOut)
			{
				//modify doc
				(itr->second).mStatus = AOSTAG_AGENT_OFFLINE;
				str << "|" << itr->first << "\t|" << (itr->second).mStatus << "\t|\n"
				<< "+---------------+---------------+\n";
			}
			else
			{
				//modify doc
				str << "|" << itr->first << "\t|" << (itr->second).mStatus << "\t\t|\n"
				<< "+---------------+---------------+\n";

			}

			itr++;
		}
		OmnScreen << str << endl;
		OmnScreen << "jozhi: " << "finish to monitor" << endl;
		mLock->unlock();
		OmnSleep(10);
	}
	return true;
}


bool
AosJimoAgentMonitor::signal(const int threadLogicId)
{
	return true;
}


bool
AosJimoAgentMonitor::checkThread(OmnString &err, const int thrdLogicId) const
{
	return true;
}


bool      	
AosJimoAgentMonitor::start()
{
	return true;
}


bool        
AosJimoAgentMonitor::stop()
{
	return true;
}

bool
AosJimoAgentMonitor::config(const AosXmlTagPtr &def)
{
	return true;
}


bool
AosJimoAgentMonitor::start(const AosXmlTagPtr &config)
{
	OmnScreen << "jozhi: " << "to start JimoAgentMonitor" << endl;
	AosXmlTagPtr monitor_conf = config->getFirstChild("jimoagent");
	if (!monitor_conf)
	{
		return true;
	}
	bool is_start = monitor_conf->getAttrBool("is_start", false);
	if (!is_start)
	{
		return true;
	}
	mLocalIp = monitor_conf->getAttrStr("local_addr");
	aos_assert_r(mLocalIp != "", false);
	if (!mReqDistr->config(monitor_conf))
	{
		OmnAlarm << "JimoAgentMonitor config error" << enderr;
	}
	mReqDistr->start();
	OmnThreadedObjPtr thisPtr(this, false);
	mThread = OmnNew OmnThread(thisPtr, "JimoAgentMonitorThrd", 0, true, true, __FILE__, __LINE__);
	mThread->start();
	return true;
}

bool
AosJimoAgentMonitor::report(
		const OmnString &report,
		const OmnString &addr)
{
	u64 crt_time = OmnGetSecond();
	map<OmnString, StatusInfo>::iterator itr;
	mLock->lock();
	itr = mJimoAgentMap.find(addr);
	if (itr == mJimoAgentMap.end())
	{
		//create doc
		OmnScreen << "jozhi: " << addr << " Online" << endl;
	}
	StatusInfo info = {crt_time, AOSTAG_AGENT_ONLINE};
	mJimoAgentMap[addr] = info;
	mLock->unlock();
	return true;

}

bool
AosJimoAgentMonitor::report(const OmnString &addr)
{
	u64 crt_time = OmnGetSecond();
	map<OmnString, StatusInfo>::iterator itr;
	mLock->lock();
	itr = mJimoAgentMap.find(addr);
	if (itr == mJimoAgentMap.end())
	{
		//create doc
		OmnScreen << "jozhi: " << addr << " Online" << endl;
	}
	StatusInfo info = {crt_time, AOSTAG_AGENT_ONLINE};
	mJimoAgentMap[addr] = info;
	mLock->unlock();
	return true;
}
