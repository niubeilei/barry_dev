////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: AppMgr.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_AppMgr_AppMgr_h
#define Omn_AppMgr_AppMgr_h

#include "AppMgr/Ptrs.h"
#include "AppMgrUtil/Ptrs.h"
#include "Debug/Rslt.h"
#include "Message/Ptrs.h"
#include "SingletonClass/SingletonTplt.h"
#include "Thread/ThreadedObj.h"
#include "Thread/Ptrs.h"
#include "Util/RCObjImp.h"
#include "Util/ValList.h"



OmnDefineSingletonClass(OmnAppMgrSingleton, 
						OmnAppMgr,
						OmnAppMgrSelf,
						OmnSingletonObjId::eAppMgr, 
						"AppMgr");


class OmnAppMgr : public OmnThreadedObj
{
	OmnDefineRCObject;
private:
	OmnMutexPtr			mLock;
	OmnCondVarPtr		mCondVar;
	OmnThreadPtr		mThread;
	OmnVList<OmnMsgPtr>	mMsgQueue;
	bool				mIsConnected;

public:
	OmnAppMgr();
	virtual ~OmnAppMgr();

	// 
	// Singleton Class interface
	//
	static OmnAppMgr *	getSelf();
	bool		start();
	bool		stop();
	bool		config(const AosXmlTagPtr &configData);

	//
	// OmnThreadedObj interface
	//
	virtual bool threadFunc(OmnThrdStatus::E &state, const OmnSPtr<OmnThread> &thread);
	virtual bool signal(const int threadLogicId);
    virtual void heartbeat(const int tid);
    virtual bool checkThread(OmnString &errmsg, const int tid) const; 

	bool			loadAppInfo();
	OmnAppInfoPtr	getAppInfo(const OmnString &appName);
	bool			isConnected() const {return mIsConnected;}
	OmnRslt			registerAppName(const OmnString &name);
	OmnRslt			registerApp(const OmnString &appName);
	OmnRslt			unregisterApp();

private:
	bool	procMsg(const OmnMsgPtr &msg);
	bool	procGetAppInfo(const OmnSmGetAppInfoReqPtr &msg);

	bool	getAppInfo(const OmnSmGetAppInfoReqPtr &msg);
	bool	getThreadInfo(const OmnSmGetAppInfoReqPtr &msg);
};

#endif
