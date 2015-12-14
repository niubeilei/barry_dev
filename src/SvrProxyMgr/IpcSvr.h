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
// Created: 04/25/2013 by Ketty 
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_SvrProxyMgr_IpcSvr_h
#define AOS_SvrProxyMgr_IpcSvr_h

#include "alarm_c/alarm.h"
#include "SEInterfaces/Ptrs.h"
#include "SEUtil/Ptrs.h"
#include "SEUtil/SeConfig.h"
#include "SvrProxyMgr/Ptrs.h"
#include "SEInterfaces/ProcessType.h"
#include "Util/Ptrs.h"
#include "XmlUtil/Ptrs.h"

#include <sys/epoll.h>
using namespace std;

#include "Thread/ThreadedObj.h"
#include "Thread/Ptrs.h"

class AosIpcSvr: public OmnThreadedObj
{
	OmnDefineRCObject;
	
	enum
	{
		eAcceptConnThrdId = 1,
	};

private:
	OmnString		mUpath;
	int				mListenSock;
	OmnThreadPtr	mConnThrd;
	AosProcessMgrPtr mProcessMgr;

	bool			mShowLog;
	bool			mIsStopping;

public:
	AosIpcSvr(
		const OmnString &dir_name,
		const AosProcessMgrPtr &proc_mgr,
		const bool showlog);
	~AosIpcSvr();

	// ThreadedObj Interface
	virtual bool    threadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread);
	virtual bool    checkThread(OmnString &err, const int thrdLogicId) const{ return true;};
	virtual bool    signal(const int threadLogicId){ return true; };
	
	bool 	startListen();
	bool	stop();
	
private:
	bool 	getPidFromUpath(
				const OmnString path,
				int &proc_id,
				u32 &logic_pid,
				AosProcessType::E &ptype);
	bool 	acceptConnThrdFunc(
				OmnThrdStatus::E &state,
				const OmnThreadPtr &thread);

};
#endif
