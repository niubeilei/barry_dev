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
// 2015/08/18 Created by Jozhi
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_StreamEngine_ServiceMgr_h
#define AOS_StreamEngine_ServiceMgr_h

#include "SingletonClass/SingletonObjId.h"
#include "SingletonClass/SingletonTplt.h"
#include "Thread/Ptrs.h"
#include "Thread/ThreadedObj.h"
#include "Rundata/Rundata.h"
#include "SEInterfaces/ServiceMgrObj.h"
#include "API/AosApi.h"
#include "Rundata/Rundata.h"
#include "StreamEngine/Service.h"

OmnDefineSingletonClass(AosServiceMgrSingleton,
						AosServiceMgr,
						AosServiceMgrSelf,
						OmnSingletonObjId::eServiceMgr,
						"ServiceMgr");


class AosServiceMgr : public OmnThreadedObj,
					  public AosServiceMgrObj
{
	OmnDefineRCObject;
public:
	enum
	{
		eMaxStartService = 50 
	};

	struct ProcInfo                    
	{                                  
		u32                 mSvrId;    
	    u32                 mLogicPid; 
		int                 mProcessId;
	};                                 

private:
	struct ServiceInfo
	{
		u64						mServiceDocid;
		u32						mLogicPid;
		u32						mSvrId;
		AosRundataPtr			mRundata;
	};

private:
	OmnMutexPtr								mLock;
	OmnCondVarPtr							mCondVar;
	OmnThreadPtr                			mStartServiceThread;
	map<u32, ProcInfo>						mProcInfos;
	deque<ServiceInfo>						mServiceQueue;
	set<u64>								mServiceSets;
	map<u32, map<u64, ServiceInfo> >		mStartService;

	map<OmnString, AosServicePtr>			mServiceMap;
	
public:
	AosServiceMgr();
	~AosServiceMgr();

    // Singleton class interface
    static AosServiceMgr *    	getSelf();
    virtual bool   	start();
    virtual bool    stop();
    virtual bool	config(const AosXmlTagPtr &def);

	// ThreadedObj interface
	virtual bool    threadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread);
	virtual bool    signal(const int threadLogicId);
	virtual bool 	addService(
						const u64 &service_docid,
						const AosRundataPtr &rdata);
	virtual bool startTaskProcCb(const u32 logic_pid, const int proc_pid, const int svr_id);
	virtual bool stopTaskProcCb(const u32 logic_pid, const int svr_id);

    //'get' and 'set' funciton
	inline map<u32, ProcInfo> getProcInfos(){return mProcInfos;};
    inline map<OmnString, AosServicePtr> &getServiceMap(){return mServiceMap;};

	bool			addService(const OmnString &name, const AosServicePtr &service);
	OmnString		getServiceInfo(const AosRundataPtr &rdata);
    bool            getProcInfo(const u32 svr_id, ProcInfo &info);

	bool 			getLogicPid(u32 &logic_pid, const int svr_id, const u64 &service_docid);
	bool    		getAllServiceInfo(const AosRundataPtr &rdata);
private:
	bool startService(
				const u64 &service_docid,
				const u32 logic_pid,
				const u32 svr_id,
				const AosRundataPtr &rdata);
	bool removeStartServiceInfoLocked(
				vector<u64> &service_docids,
				const u32 logic_pid,
				const int svr_id);

};
#endif
