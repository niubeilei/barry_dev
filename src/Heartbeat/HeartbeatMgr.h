////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: HeartbeatMgr.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////
#if 0
#ifndef Omn_Heartbeat_HeartbeatMgr_h
#define Omn_Heartbeat_HeartbeatMgr_h

#include "Debug/Rslt.h"
#include "Heartbeat/Ptrs.h"
#include "Heartbeat/HbObj.h"
#include "SingletonClass/SingletonTplt.h"
#include "SysMsg/Ptrs.h"
#include "SysObj/Ptrs.h"
#include "Thread/Ptrs.h"
#include "Thread/ThreadedObj.h"
#include "Util/String.h"
#include "Util/RCObjImp.h"
#include "Util1/TimerObj.h"
#include "Util/ValList.h"
#include "XmlParser/Ptrs.h"


OmnDefineSingletonClass(OmnHeartbeatMgrSingleton,
						OmnHeartbeatMgr,
						OmnHeartbeatMgrSelf,
						OmnSingletonObjId::eHeartbeatMgr, 
						"HeartbeatMgr");

class OmnHbObj;

class OmnHeartbeatMgr : public OmnThreadedObj,
						public OmnTimerObj,
						public OmnHbObj
{
	OmnDefineRCObject;

private:
	enum
	{
		eDefaultHbTimerSec = 5,
		eMaxMonitors = 10
	};

	enum Status
	{
		eIdle,
		eActive
	};

	Status					mStatus;
	OmnMutexPtr				mLock;
	OmnCondVarPtr			mCondVar;
	OmnThreadPtr			mThread;
	OmnHbMonitorPtr			mMonitors[eMaxMonitors];
	OmnHbObj *				mCallbacks[eMaxMonitors];
	OmnHbServerPtr			mHbServer;
	OmnHbServerPtr			mHbServer2;
	OmnDbHeartbeatPtr		mDbHeartbeat;

	int						mHbTimerSec;

public:
	OmnHeartbeatMgr();
	~OmnHeartbeatMgr();

	bool		start();
	bool		stop();
	OmnRslt		config(const OmnXmlParserPtr &conf);

	bool threadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread);
	bool signal(const int threadLogicid);

	bool checkThread(OmnString &errmsg) const;
	bool isCriticalThread() const {return true;}
	void timeout(const int timerId, const OmnString &name, void *parm);
	bool startMonitor();
	bool startMonitor(const OmnString &name, OmnString &err);
	bool stopMonitor(const OmnString &name, OmnString &err);
	bool isMonitorActive(const OmnString &name);
	bool isMonitoredGood(const int monitorId, bool &status) const;
	int	 getMonitoredStatus(OmnValList<OmnString> &names,
						    OmnValList<bool> &status);
	bool registerMonitor(const int monitorId, OmnHbObj *);

	bool restore(const int monitorId);

	OmnHbMonitorPtr	getMonitor(const int monitorId);
	OmnHbMonitorPtr	getMonitor(const OmnString &name);
	virtual void	heartbeatFailed(const int monitorId); 
	virtual void	heartbeatRestored(const int monitorId); 
	virtual void	controlMsgRead(const OmnString &,
									const OmnConnBuffPtr &);
	virtual OmnRslt     config(const OmnSysObjPtr &def);
	virtual OmnString   getSysObjName() const {return "OmnHeartbeat";}
	virtual OmnSingletonObjId::E getSysObjId() const
						{return OmnSingletonObjId::eHeartbeatMgr;}
};

#endif
#endif
