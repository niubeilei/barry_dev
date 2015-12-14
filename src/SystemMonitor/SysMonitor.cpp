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
#include "SystemMonitor/SysMonitor.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "API/AosApi.h"
#include "Debug/Debug.h"
#include "Porting/Sleep.h"
#include "Rundata/Rundata.h"
#include "SeLogClient/SeLogClient.h"
#include "SingletonClass/SingletonImpl.cpp"
#include "SysTrans/ThreadMonitorTrans.h"
#include "SysTrans/AskWheatherDeathTrans.h"
#include "SysTrans/TellServerDeathTrans.h"
#include "Thread/ThreadMgr.h"
//#include "TransUtil/AsyncReqTransMgr.h"
#include "Util/OmnNew.h"
#include "Util/File.h"
#include "XmlUtil/SeXmlParser.h"

#include <fstream>

OmnSingletonImpl(AosSysMonitorSingleton, AosSysMonitor, AosSysMonitorSelf, "AosSysMonitor");

AosSysMonitor::AosSysMonitor()
:
mLock(OmnNew OmnMutex()),
mSendTransNum(0)
{
	mSvrIds = AosGetServerIds();
	mSelfId = AosGetSelfServerId();

	mServerNums = mSvrIds.size();
	for(int i=0; i<mServerNums; i++)
	{
		ServerMsgPtr server_msg = OmnNew ServerMsg();
		mServerMsgs.insert(make_pair(mSvrIds[i], server_msg));
	}
	
	mDftLogStr = "<sysmonitor ";
	mDftLogStr << AOSTAG_LOGNAME << "=\"sysmonitor\" "
			   << AOSTAG_PHYSICAL_SERVER_ID << "=\"" << mSelfId << "\">";
	mLogStr = mDftLogStr;

}


AosSysMonitor::~AosSysMonitor()
{
}


bool
AosSysMonitor::start()
{
	OmnThreadedObjPtr thisPtr(this, false);
	mThread = OmnNew OmnThread(thisPtr, "SysMonitorThread", 0, true, true, __FILE__, __LINE__);
	mStartTime = OmnGetSecond();
	mStartLogTime = OmnGetSecond();
	mThread->start();
	return true;
}


bool 
AosSysMonitor::stop()
{
	//mThread->stop();
	return true;
}


bool 
AosSysMonitor::config(const AosXmlTagPtr &config)
{
	aos_assert_r(config, false);
	
	AosXmlTagPtr sysmonitor = config->getFirstChild(AOSCONFIG_SYSMONITOR); 
	aos_assert_r(sysmonitor, false);

	mHeartBeatMSec = sysmonitor->getAttrInt("zky_heartbeat", eDftSleepSec); // msecond

	mCheckTime = mHeartBeatMSec * 30; //ms
	
	mDeathPercent = sysmonitor->getAttrInt("zky_death_percent", eDftDeathPercent);

	mCreateLogTime = sysmonitor->getAttrU32("zky_create_log_time", eDftCreateLogTime); //second
	return true;
}

bool
AosSysMonitor::threadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread)
{
	while (state == OmnThrdStatus::eActive)
	{
		OmnString errorThrds;
		OmnThreadMgr::getSelf()->getErrorThread(errorThrds);
		
		bool rslt = false;
		rslt = addReq(errorThrds);
		aos_assert_r(rslt, false);
		
		mThreadStatus = true;

		u32 time = OmnGetSecond();
		mLock->lock();
		if (time - mStartLogTime >= mCreateLogTime)
		{
			createLog();
			mStartLogTime = time;
		}
		mLock->unlock();

		//OmnSleep(1);
		OmnMsSleep(mHeartBeatMSec);
	}
	return true;
}

bool
AosSysMonitor::composeLogStr(const AosBuffPtr &errorThrds)
{
	int svr_id = errorThrds->getInt(-1);
	int trans_num = errorThrds->getInt(0);
	bool has_error = errorThrds->getU8(0);
	OmnString errorMsg = "";
	if (has_error)
	{
		errorMsg = errorThrds->getOmnStr("");
	}
	mLock->lock();

	map<int, ServerMsgPtr>::iterator itr = mServerMsgs.find(svr_id);
	aos_assert_rl(itr != mServerMsgs.end(), mLock, false);
	
	int pos = trans_num - itr->second->mStartPos;
	if (pos < 0)
	{
		mLock->unlock();
		return true;
	}

	if (itr->second->mActivePos < trans_num)
	{
		itr->second->mActivePos = trans_num;
	}

	if(pos < eDftLogSec)
	{
		itr->second->mStatus.set(pos);
	}
	
	if (errorMsg != "")
	{
		getErrorMsg(errorMsg, itr->second->mErrorMaps);
	}

	itr->second->mErrorThrds << errorMsg;

	u32 sec = OmnGetSecond();
	if (mSelfId == svr_id && (int)(sec - mStartTime) >= mCheckTime / 1000)
	{
		checkServer();
		mStartTime = sec;
	}
	mLock->unlock();

	return true;
}

bool
AosSysMonitor::getErrorMsg(
		OmnString &errorMsg,
		map<OmnString, OmnString> &hasErrMsg)
{
	AosXmlTagPtr xml = AosXmlParser::parse(errorMsg AosMemoryCheckerArgs);    
	aos_assert_r(xml, false);

	errorMsg = "";
	map<OmnString, OmnString>::iterator itr;
	AosXmlTagPtr child = xml->getFirstChild(true);
	while(child)
	{
		OmnString key = child->getAttrStr("zky_key", "");
		aos_assert_r(key != "", false);

		itr = hasErrMsg.find(key);
		if (itr == hasErrMsg.end())
		{
			OmnString error = child->toString();
			errorMsg << error;
			hasErrMsg.insert(make_pair(key, error));
		}

		child = xml->getNextChild();
	}

	return true;
}

bool
AosSysMonitor::checkServer()
{
	for(int i=0; i<mServerNums; i++)
	{
		map<int, ServerMsgPtr>::iterator itr = mServerMsgs.find(mSvrIds[i]);
		aos_assert_r(itr != mServerMsgs.end(), false);
		
		size_t active_nums = itr->second->mStatus.count();
		aos_assert_r(active_nums <= eDftLogSec, false);
		itr->second->mStatus.reset();
		itr->second->mStartPos = itr->second->mActivePos+1;
		itr->second->mErrorMaps.clear();
		
		OmnString msg = "<record server_id =\"";
		msg << mSvrIds[i] << "\" ";
		if ((active_nums * 100)/eDftLogSec <= (100 - mDeathPercent))
		{
			//server i is death
			msg << " server_status=\"isDeath\">";
			itr->second->mTempDeath = true;
			askOtherServer(mSvrIds[i]);
		}
		else
		{
			msg << " server_status=\"isActive\">";
		}
		msg << itr->second->mErrorThrds << "</record>";
		itr->second->mErrorThrds = "";
		mLogStr << msg;
	}

	return true;
}

bool
AosSysMonitor::askOtherServer(const int deathid)
{
	// Ketty 2013/07/20
	//AosAsyncReqCallerPtr thisPtr(this, false);
	//for(int i=0; i<mServerNums; i++)
	//{
	//	AosAsyncReqTransPtr trans = OmnNew AosAskWheatherDeathTrans(OmnGetSecond(), deathid, mSvrIds[i], false, false);
	//	trans->setCaller(thisPtr);
	//	AosAsyncReqTransMgr::getSelf()->addRequestAsync(trans, OmnApp::getRundata());
	//}
	for(int i=0; i<mServerNums; i++)
	{
		AosTransPtr trans = OmnNew AosAskWheatherDeathTrans(OmnGetSecond(), deathid, mSvrIds[i], false, false);
		AosSendTransAsyncResp(OmnApp::getRundata(), trans);
	}

	return true;
}

bool
AosSysMonitor::tellOtherServer(const int deathid)
{
	for(int i=0; i<mServerNums; i++)
	{
		AosTransPtr trans = OmnNew AosTellServerDeathTrans(deathid, mSvrIds[i], false, false);
		bool rslt = addTrans(trans);
		aos_assert_r(rslt, false);
	}

	return true;
}

void
//AosSysMonitor::callback(AosAsyncReqTransPtr &trans)
AosSysMonitor::askDeathCallback(
		const AosTransPtr &trans,
		const AosBuffPtr &resp,
		const bool svr_death)
{
	// Ketty 2013/07/20
	if(svr_death)
	{
		OmnScreen << "send Trans failed. svr death."
			<< "; trans_id:" << trans->getTransId().toString()
			<< "; death_svr_id:" << trans->getToSvrId()
			<< endl;
		return;
	}

	aos_assert(resp);

	u32 starttime = resp->getU32(0);
	int deathid = resp->getInt(0);
	int askserver = resp->getInt(0);
	bool death = resp->getU8(0);

	mLock->lock();
	map<int, ServerMsgPtr>::iterator itr = mServerMsgs.find(deathid);
	aos_assert(itr != mServerMsgs.end());

	if (death)
	{
		itr->second->mDeath.set(askserver);
	}

	if (OmnGetSecond() - starttime >= eAskWaitSec)
	{
		int death_nums = itr->second->mDeath.count();
		aos_assert_l(death_nums < mServerNums, mLock);
		itr->second->mDeath.reset();
		if ((death_nums * 100) / mServerNums >= mDeathPercent)
		{
			// death death death 
			// deathid server is surely dead
			// tell all the other servers it is dead
			tellOtherServer(deathid);
		}
	}
	mLock->unlock();
}

void
AosSysMonitor::setServerDeath(const int serverid)
{
	mLock->lock();
	map<int, ServerMsgPtr>::iterator itr = mServerMsgs.find(serverid);
	aos_assert_l(itr != mServerMsgs.end(), mLock);
	itr->second->mServerDeath = true;
	mLock->unlock();
}

bool
AosSysMonitor::getStatus(const int serverid)
{
	bool death;
	mLock->lock();
	map<int, ServerMsgPtr>::iterator itr = mServerMsgs.find(serverid);
	aos_assert_rl(itr != mServerMsgs.end(), mLock, false);
	death = itr->second->mTempDeath;
	mLock->unlock();

	return death;
}

bool
AosSysMonitor::createLog()
{
	if(mLogStr == mDftLogStr) 
		return true;

	mLogStr << "</sysmonitor>";

	bool rslt = AosAddLog(OmnApp::getRundata(), AOSCTNR_SYSLOGCTNR,
					"sysmonitor", mLogStr);
	aos_assert_r(rslt, false);

	OmnScreen << "~~~~ systemMonitor addLog : " << mLogStr << endl;
	mLogStr = mDftLogStr;
	return true;
}

bool
AosSysMonitor::addReq(const OmnString &errorThrds)
{
	
	mLock->lock();
	int sendnum = mSendTransNum;
	mSendTransNum++;
	mLock->unlock();

	int64_t buffsize = errorThrds.length() + sizeof(int) *2;
	AosBuffPtr buff = OmnNew AosBuff(buffsize AosMemoryCheckerArgs);
//	buff->setDataLen(buffsize);
	buff->setInt(mSelfId);
	buff->setInt(sendnum);

OmnScreen <<"@@@@@@ " << mSelfId << " : " << sendnum << " : " << buffsize << " : " << errorThrds.length() << " " << errorThrds <<endl;
	if (errorThrds != "")
	{
		buff->setU8(true);
		buff->setOmnStr(errorThrds);
	}
	else
	{
		buff->setU8(false);
	}

	for(u32 i=0; i<mSvrIds.size(); i++)
	{
		AosTransPtr trans = OmnNew AosThreadMonitorTrans(mSvrIds[i], buff, false, false);
		bool rslt = addTrans(trans);
		aos_assert_r(rslt, false);
	}
	
	return true;
}

bool
AosSysMonitor::addTrans(const AosTransPtr &trans)
{
	bool rslt = AosSendTrans(OmnApp::getRundata(), trans);
	aos_assert_r(rslt, false);
	return true;
}


