////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
//
// Modification History:
// 	Created: 05/09/2010 by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_JimoAgentMonitor_JimoAgentMonitor_h
#define AOS_JimoAgentMonitor_JimoAgentMonitor_h

#include "Thread/ThreadedObj.h"
#include "XmlUtil/XmlTag.h"
#include "Proggie/ReqDistr/ReqDistr.h"
#include "Proggie/ReqDistr/Ptrs.h"

OmnDefineSingletonClass(AosJimoAgentMonitorSingleton,
						AosJimoAgentMonitor,
						AosJimoAgentMonitorSelf,
						OmnSingletonObjId::eJimoAgentMonitor,
						"AosJimoAgentMonitor");

#define AOSTAG_AGENT_ONLINE		"online"
#define AOSTAG_AGENT_OFFLINE	"offline"

class AosJimoAgentMonitor: virtual public OmnThreadedObj
{
	OmnDefineRCObject;

private:
	struct StatusInfo
	{
		u64 		mUpdateTime;
		OmnString 	mStatus;
	};
	enum
	{
		eTimeOut = 5 
	};
	map<OmnString, StatusInfo>	mJimoAgentMap;		
	OmnThreadPtr        		mThread;
	OmnMutexPtr					mLock;
	AosReqDistrPtr				mReqDistr;
	OmnString					mLocalIp;
public:
	AosJimoAgentMonitor();
	~AosJimoAgentMonitor();

    // Singleton class interface
    static AosJimoAgentMonitor*  	getSelf();
    virtual bool      	start();
    virtual bool        stop();
	virtual bool        config(const AosXmlTagPtr &def);
	// ThreadedObj Interface
	virtual bool    threadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread);
	virtual bool    checkThread(OmnString &err, const int thrdLogicId) const;
	virtual bool    signal(const int threadLogicId);
	bool start(const AosXmlTagPtr &config);
	bool report(const OmnString &report, const OmnString &addr);
	bool report(const OmnString &addr);
	OmnString getLocalIp() { return mLocalIp; }
};
#endif

