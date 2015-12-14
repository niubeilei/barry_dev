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
// 10/12/2010	Created by Chen Ding
// 2014/01/28 Turned off by Chen Ding
////////////////////////////////////////////////////////////////////////////
#if 0
#ifndef AOS_DataSync_DataSyncClt_h
#define AOS_DataSync_DataSyncClt_h

#include "DataSync/DsTags.h"
#include "DataSync/Ptrs.h"
#include "HealthCheck/Ptrs.h"
#include "Thread/Ptrs.h"
#include "Thread/CondVar.h"
#include "Thread/ThreadedObj.h"
#include "Util/RCObject.h"
#include "SingletonClass/SingletonObjId.h"
#include "SingletonClass/SingletonTplt.h"
#include "Util/RCObjImp.h"
#include "Util/IpAddr.h"
#include "Util/HashUtil.h"
#include "UtilComm/Ptrs.h"
#include "UtilComm/TcpListener.h"
#include "TransUtil/Ptrs.h"
#include "XmlUtil/SeXmlParser.h"
#include "XmlUtil/XmlTag.h"

#include <map>
#include <queue>
using namespace std;

OmnDefineSingletonClass(AosDataSyncCltSingleton,
						AosDataSyncClt,
						AosDataSyncCltSelf,
						OmnSingletonObjId::eDataSyncClt,
						"DataSyncClt");

class AosDataSyncClt: virtual public OmnThreadedObj, public OmnTcpListener 
{
	OmnDefineRCObject;

private:
	enum
	{
		eNumPorts = 5,
		eMonitorFreqSec = 60,
		eRespTimer = 20,
		eQueueLength = 1000
	};

	OmnMutexPtr			mLock;
	OmnCondVarPtr       mCondVar;
	OmnString			mClientId;
	OmnThreadPtr		mReqThread;
	OmnThreadPtr		mMonitorThread;
	OmnString			mAddr;
	int					mPort;
	OmnTcpCommSvrPtr	mConn;
	AosDsTransPtr 		mTran;
	OmnString			mType;
	bool				mFinishFlag;
	bool				mStopPush;
	u32					mLogid;
	//AosDsTransFileMgrPtr  mTransFile;		// Ketty 2013/02/22
	OmnTcpClientPtr		mClt;

	u32 				mTid;

	queue<OmnString> mReqQueue;

public:
	AosDataSyncClt();
	AosDataSyncClt(const AosXmlTagPtr &def);
	~AosDataSyncClt();
	
	
	 // Singleton class interface
	 static AosDataSyncClt*    getSelf();
	 virtual bool        start(){return true;}
	 virtual bool        stop(){return true;}
	 virtual OmnString   getSysObjName() const {return "AosDataSyncClt";}
	 virtual bool     config(const AosXmlTagPtr &def){return true;}

	// OmnThreadedObj Interface
	virtual bool    threadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread);
	virtual bool    signal(const int threadLogicId);
	virtual bool    checkThread(OmnString &err, const int thrdLogicId) const;

	// OmnCommListener Interface
	virtual OmnString   getTcpListenerName() const {return "TestServer";}
	virtual void        msgRecved(const OmnConnBuffPtr &buff,
			                      const OmnTcpClientPtr &conn);
	virtual void 		connCreated(const OmnTcpClientPtr &conn){}
	virtual void 		connClosed(const OmnTcpClientPtr &conn){}

	/////////
	bool    start(const AosXmlTagPtr &def);
	bool    requestFinished(const u64 &request_id);
	OmnTcpClientPtr getConn();

private:
	bool	createConnection();
	bool	syncData(const OmnString &request);
	bool	addRequest(const OmnString &data);
	bool	reqThreadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread);
	bool    monitorThreadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread);
	bool    procMsg(const OmnConnBuffPtr &buff);
	bool	procResp(const AosXmlTagPtr &msg);
	bool	beginSync();
};

#endif
#endif
