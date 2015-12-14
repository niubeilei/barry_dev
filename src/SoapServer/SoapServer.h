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
// handle the SEServer send request to MsgServer 
//
// Modification History:
// 02/22/2012 Created by Linda 
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_SoapServer_SoapServer_h
#define AOS_SoapServer_SoapServer_h

#include "MsgProc/MsgProc.h"
#include "SEInterfaces/SoapServerObj.h"
#include "SingletonClass/SingletonObjId.h"
#include "SingletonClass/SingletonTplt.h"
#include "SoapServer/Ptrs.h"
#include "Thread/ThreadedObj.h"
#include "Util/String.h"
#include "Util/HashUtil.h"
#include "XmlUtil/Ptrs.h"
#include "XmlUtil/XmlTag.h"
#include <queue>


OmnDefineSingletonClass(AosSoapServerSingleton,
                        AosSoapServer,
                        AosSoapServerSelf,
                        OmnSingletonObjId::eSoapServer,
                        "SoapServer");

class AosSoapServer : public AosSoapServerObj, public OmnThreadedObj
{
	OmnDefineRCObject;

	typedef map<OmnString, AosMsgProcPtr> ProcMap_t;
	typedef map<OmnString, AosMsgProcPtr>::iterator ProcMapItr_t;

private:
	enum
	{
		eDftNumThreads = 1,
		eMaxThreads = 50,
		eDftMaxQueueSize = 10000,
		eMaxQueueSizeLimit = 100000
	};

	struct Request
	{
		OmnString appid;
//		AosSoapConnPtr conn;
		AosXmlTagPtr msg;

		Request(const OmnString &a,
			//	const AosSoapConnPtr &c,
				const AosXmlTagPtr &m)
		:
		appid(a),
//		conn(c),
		msg(m)
		{
		}
	};

	OmnMutexPtr				mLock;
	OmnCondVarPtr			mCondVar;
	ProcMap_t				mProcMap;
	queue<Request>			mMsgQueue;
	vector<OmnThreadPtr>	mThreads;
	vector<AosRundataPtr>	mRundata;
	bool					mStarted;
	bool					mConfigured;
	u32						mMaxQueueSize;
	int						mNumThrds;

public:
	AosSoapServer();
	~AosSoapServer();

    // Singleton class interface
    static AosSoapServer*   getSelf();
    virtual bool        	start();
    virtual bool        	stop();
    virtual bool			config(const AosXmlTagPtr &def);

	// ThreadedObj interface
	virtual bool	threadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread);
	virtual bool	signal(const int threadLogicId);
    virtual bool    checkThread(OmnString &err, const int thrdLogicId) const;

	//bool msgReceived(	const OmnString &appid, 
	//					const AosSoapConnPtr &conn, 
	//					const AosXmlTagPtr &msg);
	
	bool	procMsg(const OmnString &reqid, const OmnString &input, OmnString &output);

	// AosSoapServerObj interface
	virtual bool registerMsgProc(const OmnString &appid, const AosMsgProcPtr &proc);

private:
};
#endif

