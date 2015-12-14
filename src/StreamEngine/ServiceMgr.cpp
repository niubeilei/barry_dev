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
//  This class is used to create docs. It will:
//      1. Parse the doc to collect the data;
//      2. Add all the words into the database;
//      3. Add the doc into the database;
//
// Modification History:
// 2015/08/18 Created by Jozhi
////////////////////////////////////////////////////////////////////////////
#include "StreamEngine/ServiceMgr.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "SingletonClass/SingletonImpl.cpp"
#include "SysMsg/StartDynamicProcMsg.h"
#include "JobTrans/StartServiceTrans.h"
#include "StreamEngine/StreamDataProc.h"
#include "StreamEngine/Ptrs.h"
#include "JobTrans/GetServiceInfoTrans.h"

static u32 sgMaxStartService = AosServiceMgr::eMaxStartService;

OmnSingletonImpl(AosServiceMgrSingleton,
                 AosServiceMgr,
                 AosServiceMgrSelf,
                "AosServiceMgr");

AosServiceMgr::AosServiceMgr()
:
mLock(OmnNew OmnMutex()),
mCondVar(OmnNew OmnCondVar())
{
}


AosServiceMgr::~AosServiceMgr()
{
}


bool
AosServiceMgr::start()
{
	OmnThreadedObjPtr thisPtr(this, false);
	mStartServiceThread = OmnNew OmnThread(thisPtr, "ServiceMgrStartServiceThrd", 0, true, true, __FILE__, __LINE__);
	mStartServiceThread->start();
	u32 num = AosGetNumPhysicals();
	for (u32 i=0; i<num; i++)
	{
	OmnScreen << "======================send start task process:" << endl;
		AosAppMsgPtr msg = OmnNew AosStartDynamicProcMsg(AosProcessType::eService, AosGetSelfServerId(), i);
	    aos_assert_r(msg, false);
		AosSendMsg(msg);
	}
	return true;
}


bool
AosServiceMgr::config(const AosXmlTagPtr &config)
{
	return true;
}


bool
AosServiceMgr::stop()
{
    return true;
}


bool    
AosServiceMgr::threadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread)
{
	u32 svr_id = 0;
	u64 service_docid = 0;
	u32 logic_pid = 0;
	ServiceInfo service_info;
	ProcInfo proc_info;
	AosRundataPtr rdata;
	map<u32, map<u64, ServiceInfo> >::iterator itr;
	while (state == OmnThrdStatus::eActive)
	{
		mLock->lock();
		while (!mServiceQueue.empty())
		{
			service_info = mServiceQueue.front();
			rdata = service_info.mRundata;
			svr_id = service_info.mSvrId;
			service_docid = service_info.mServiceDocid;
			aos_assert_rl(mProcInfos.find(svr_id) != mProcInfos.end(), mLock, false);
			proc_info = mProcInfos[svr_id];
			logic_pid = proc_info.mLogicPid;
			service_info.mLogicPid = logic_pid;
			startService(service_docid, logic_pid, svr_id, rdata);
			mServiceQueue.pop_front();

			itr = mStartService.find(svr_id);
			if (itr == mStartService.end())
			{
				map<u64, ServiceInfo> tmp;
				mStartService[svr_id] = tmp;
				itr = mStartService.find(svr_id);
			}
			aos_assert_r(itr != mStartService.end(), false);
			(itr->second)[service_docid] = service_info;
		}
		mCondVar->wait(mLock);
		mThreadStatus = true;
		mLock->unlock();
	}
	return true;
}

bool    
AosServiceMgr::signal(const int threadLogicId)
{
	return true;
}

bool
AosServiceMgr::startService(
				const u64 &service_docid,
				const u32 logic_pid,
				const u32 svr_id,
				const AosRundataPtr &rdata)
{
	AosTransPtr trans = OmnNew AosStartServiceTrans(service_docid, svr_id, logic_pid);
	aos_assert_r(trans, false);
OmnScreen << "============================send start service trans: " << service_docid << " : " << logic_pid  << " : " << trans->getTransId().toString() << endl;
	bool rslt = AosSendTrans(rdata, trans);

	OmnScreen << "start a new service trans, service_docid is: " << 
		service_docid << " logic_pid is: " << logic_pid << endl;
	aos_assert_r(rslt, false);
	return true;
}

bool
AosServiceMgr::addService(
			const u64 &service_docid,
			const AosRundataPtr &rdata)
{
	mLock->lock();
	if (mServiceSets.find(service_docid) != mServiceSets.end())
	{
		OmnAlarm << "this service is started, service_docid: " << service_docid << enderr;
		mLock->unlock();
		return false;
	}
	if (mServiceSets.size() >= sgMaxStartService)
	{
		OmnAlarm << "start service is too manay, max: " << sgMaxStartService 
			<< " service_docid: " << service_docid << enderr;
		mLock->unlock();
		return false;
	}
	u32 num = AosGetNumPhysicals();
	for (u32 i=0; i<num; i++)
	{
		ServiceInfo info;
		info.mServiceDocid = service_docid;
		info.mRundata = rdata->clone(AosMemoryCheckerArgsBegin);
		info.mSvrId = i;
		mServiceQueue.push_back(info);
		if (mProcInfos.find(i) == mProcInfos.end())
		{
			AosAppMsgPtr msg = OmnNew AosStartDynamicProcMsg(AosProcessType::eService, AosGetSelfServerId(), i);
			aos_assert_rl(msg, mLock, false);
			AosSendMsg(msg);
		}
	}
	mCondVar->signal();
	mLock->unlock();
	return true;
}


bool
AosServiceMgr::startTaskProcCb(const u32 logic_pid, const int proc_pid, const int svr_id)
{
	aos_assert_r(svr_id >= 0, false);
	ProcInfo info;
	info.mSvrId = svr_id;
	info.mLogicPid = logic_pid;
	info.mProcessId = proc_pid;
OmnScreen << "ServiceMgr startTaskProcCb"
	<< " ,svr_id: " << svr_id
	<< " , logic_pid: " << logic_pid
	<< " , process_id: " << proc_pid
	<< endl;
	mLock->lock();
	mProcInfos[(u32)svr_id] = info;
	mCondVar->signal();
	mLock->unlock();
	return true;
}

bool
AosServiceMgr::stopTaskProcCb(const u32 logic_pid, const int svr_id)
{
	OmnScreen << "Alarm service.exe is stop, svr_id: " << svr_id << " ,logic_pid: " << logic_pid << endl;
	u64 service_docid = 0;
	vector<u64> service_docids;
	mLock->lock();
	bool rslt = removeStartServiceInfoLocked(service_docids, logic_pid, svr_id);
	aos_assert_rl(rslt, mLock, false);
	if (mProcInfos.find(svr_id) != mProcInfos.end())
	{
		mProcInfos.erase(svr_id);
	}
	for (u32 i=0; i<service_docids.size(); i++)
	{
OmnScreen << "start service docid: " << service_docid 
		  << " ,svr_id: " << svr_id 
		  << " ,logic_pid: " << logic_pid << endl;
	}
	mLock->unlock();
	return true;
}

bool
AosServiceMgr::removeStartServiceInfoLocked(
					vector<u64> &service_docids,
					const u32 logic_pid,
					const int svr_id)
{
	map<u32, map<u64, ServiceInfo> >::iterator itr = mStartService.find(svr_id);
	aos_assert_r(itr != mStartService.end(), false);
	map<u64, ServiceInfo>::iterator i_itr = (itr->second).begin();
	map<u64, ServiceInfo>::iterator tmp_itr;
	while(i_itr != (itr->second).end())
	{
		if ((i_itr->second).mLogicPid == logic_pid)
		{
			service_docids.push_back(i_itr->first);
			tmp_itr = i_itr;
			i_itr++;
			(itr->second).erase(tmp_itr);
		}
		else
		{
			i_itr++;
		}
	}
	return true;
}


bool			
AosServiceMgr::addService(const OmnString &name, const AosServicePtr &service)
{
	mLock->lock();
	mServiceMap[name] = service;
	mLock->unlock();
	return true;
}

bool 
AosServiceMgr::getAllServiceInfo(const AosRundataPtr &rdata)
{
	bool rslt = false;
	AosTransPtr trans;
	OmnString msg;
	mLock->lock();
	map<u32, map<u64, ServiceInfo> >::iterator itr = mStartService.begin();
	map<u64, ServiceInfo>::iterator s_itr;
	while(itr != mStartService.end())
	{

		s_itr = (itr->second).begin();
		while(s_itr != (itr->second).end())
		{
			ServiceInfo info = s_itr->second;
			AosBuffPtr resp_buff;
			bool timeout = false;
			trans = OmnNew AosGetServiceInfoTrans(itr->first, info.mLogicPid);
			rslt = AosSendTrans(rdata, trans, timeout, resp_buff);
			aos_assert_r(rslt, false);
			rslt = resp_buff->getU8(0);
			aos_assert_r(rslt, false);
			msg << resp_buff->getOmnStr("");
			s_itr++;
		}
		itr++;
	}
	rdata->setJqlMsg(msg);
	mLock->unlock();
	return true;

}


OmnString		
AosServiceMgr::getServiceInfo(const AosRundataPtr &rdata)
{
	mLock->lock();
	
	map<OmnString, AosServicePtr>::iterator itr;
	map<OmnString, AosDataProcObjPtr>::iterator dpItr;

	itr = mServiceMap.begin();
	OmnString msg = "";
	while(itr != mServiceMap.end())
	{
		OmnString fname = itr->first;
		AosServicePtr service = mServiceMap[fname];
        aos_assert_r(service,"There is no service");

		//output service name
		OmnScreen << "Service Name: " << fname << endl;
		msg << "Service Name: " << fname << "\n";

		map<OmnString, AosDataProcObjPtr> dataproc_map = service->getDataProcObjMap();

		//output dataproc info
		dpItr = dataproc_map.begin();
		while(dpItr != dataproc_map.end())
		{
			OmnString strProc = dpItr->first;
			AosDataProcObjPtr dataProc = dpItr->second;
			AosStreamDataProc* streamDataProc = dynamic_cast<AosStreamDataProc*>(dataProc.getPtr());
			
			u64 count = streamDataProc->getCount();
			//procTime += streamDataProc->getTimeCounter()/1000;
			msg <<"        " << strProc 
				<< ": processed " << count << " data units." << "\n";
			dpItr++;
		}

		itr++;
	}
     	
    mLock->unlock();
	return msg;
}

bool                                                                 
AosServiceMgr::getProcInfo(const u32 svr_id, ProcInfo &info)         
{                                                                    
	mLock->lock();                                                   
	aos_assert_r(mProcInfos.find(svr_id) != mProcInfos.end(), false);
	info = mProcInfos[svr_id];                                       
	mLock->unlock();                                                 
	return true;                                                     
}                                                                    


bool 			
AosServiceMgr::getLogicPid(
		u32 &logic_pid, 
		const int svr_id, 
		const u64 &service_docid)
{
	mLock->lock();
	map<u32, map<u64, ServiceInfo> >::iterator itr = mStartService.find(svr_id);
	if (itr == mStartService.end())
	{
		mLock->unlock();
		return false;
	}
	if (service_docid == 0)
	{
		aos_assert_r(mProcInfos.find(svr_id) != mProcInfos.end(), false);
		ProcInfo info = mProcInfos[svr_id];                                       
		logic_pid = info.mLogicPid;	
	}
	else
	{
		map<u64, ServiceInfo>::iterator s_itr = (itr->second).find(service_docid);
		if (s_itr == (itr->second).end())
		{
			mLock->unlock();
			return false;
		}
		logic_pid = (s_itr->second).mLogicPid;
	}
	mLock->unlock();
	return true;
}
