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
// 2010/12/25	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Omn_Heartbeat_HeartbeatMgr_h
#define Omn_Heartbeat_HeartbeatMgr_h

#include "Debug/Rslt.h"
#include "Heartbeat/Ptrs.h"
#include "Heartbeat/HbObj.h"
#include "Thread/Ptrs.h"
#include "Thread/ThreadedObj.h"
#include "Util/String.h"
#include "Util/RCObjImp.h"
#include "Util1/TimerObj.h"
#include "UtilComm/CommListener.h"



class OmnHeartbeatMgr : public OmnThreadedObj,
						public OmnTimerObj,
						public OmnCommListener,
						public AosHeartbeatMgrObj
{
	OmnDefineRCObject;

private:
	enum
	{
		eDftHeartbeatFreq = 1,

		eStatusSuccess = 1
	};

	struct Monitored
	{
		OmnString	ip_addr_str;
		int			port;
		OmnString	name;
		OmnIpAddr	ip_addr;

		Monitored(const OmnString &ip_str, const int p, const OmnString &n)
		:
		ip_addr_str(ip_str),
		port(p),
		name(name)
		{
			ip_addr.set(ip_str);
		}
	};

	OmnMutexPtr			mLock;
	OmnThreadPtr		mThread;
	int					mHeartbeatFreq;
	u64					mModuleId;
	vector<Monitored>	mMonitored;
	int *				mMonitoredIdx;
	OmnUdpCommPtr		mComm;
	bool				mHeartbeatSent;
	char *				mResponded;

public:
	OmnHeartbeatMgr();
	~OmnHeartbeatMgr();

	bool		start();
	bool		stop();
	OmnRslt		config(const OmnXmlParserPtr &conf);

	// OmnThreadedObj interface
	bool threadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread);
	bool signal(const int threadLogicid);

	// OmnCommListener Interface
	virtual bool        msgRead(const OmnConnBuffPtr &buff);
	virtual OmnString   getCommListenerName() const {return "heartbeat";}
	virtual void        readingFailed();

	void	setModuleid(const u64 &id) {mModuleid = id;}
	u64		getModuleid() const {return mModuleid;}
	bool 	checkThread(OmnString &errmsg) const;
	bool 	isCriticalThread() const {return true;}
	virtual OmnSingletonObjId::E getSysObjId() const
						{return OmnSingletonObjId::eHeartbeatMgr;}
};

#endif

