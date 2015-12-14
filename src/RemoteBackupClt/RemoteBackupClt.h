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
// 02/12/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_RemoteBackupClt_RemoteBackupClt_h
#define AOS_RemoteBackupClt_RemoteBackupClt_h

#include "IdGen/Ptrs.h"
#include "IILUtil/IILId.h"
#include "IILIDIdGen/Ptrs.h"
#include "RemoteBackupClt/Ptrs.h"
#include "RemoteBackupUtil/RemoteBkType.h"
#include "Rundata/Ptrs.h"
#include "SEInterfaces/RemoteBkCltObj.h"
#include "SingletonClass/SingletonObjId.h"
#include "SingletonClass/SingletonTplt.h"
#include "Thread/Ptrs.h"
#include "Thread/ThreadedObj.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/IpAddr.h"
#include "UtilComm/Ptrs.h"
#include <queue>
using namespace std;

/*OmnDefineSingletonClass(AosRemoteBackupCltSingleton,
                        AosRemoteBackupClt,
                        AosRemoteBackupCltSelf,
                        OmnSingletonObjId::eRemoteBackupClt,
                        "RemoteBackupClt");
*/
class AosRemoteBackupClt : public OmnThreadedObj, public AosRemoteBkCltObj
{
	OmnDefineRCObject;
	
	enum
	{
		eInitTransid = 1000,
		eTransidBatchSize = 1000,
		eDftSleepSecBeforeReconn = 5,
		eDftQueueFullSize = 500 
	};

private:
	OmnMutexPtr		mLock;
	OmnCondVarPtr	mCondVar;
	OmnThreadPtr	mThread;
	bool			mUseQueue;
	u64				mStartTransid;
	u64				mTransidPrefix;
	bool			mHasRecordsInDb;
	OmnTcpClientPtr	mConn;
	OmnIpAddr		mRemoteAddr;
	int				mRemotePort;
	int				mNumPorts;
	u32				mQueueFullSize;
	int				mSleepSecBeforeReconn;
	int				mServerId;
	int				mSystemId;
	AosU64IdGenForIILPtr		mIdGen;
	queue<AosRemoteBkReqPtr>	mRequests;
	OmnString		mTableName;

public:
	AosRemoteBackupClt();
	AosRemoteBackupClt(const AosXmlTagPtr &config);
	~AosRemoteBackupClt();

    // Singleton class interface
    /*
	static AosRemoteBackupClt*    getSelf();
    virtual bool        	start();
    virtual bool        	stop();
    virtual bool			config(const AosXmlTagPtr &def);
    */ 

	// AosThreadedObj interface
	virtual bool	threadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread);
	virtual bool	signal(const int threadLogicId);
    virtual bool    checkThread(OmnString &err, const int thrdLogicId) const;

	// AosRemoteBkCltObj Interface
	virtual bool docCreated(const AosXmlTagPtr &doc, const AosRundataPtr &rdata);
	virtual bool docDeleted(const AosXmlTagPtr &doc, const AosRundataPtr &rdata);
	virtual bool docModified(const AosXmlTagPtr &doc, const AosRundataPtr &rdata);
	virtual bool saveIILs(
				const u64 &iilid,
				const AosBuffPtr &tmpfilebuff, 
				const AosRundataPtr &rdata);
	virtual bool addWord(const u64 &iilid, const OmnString &word, const AosRundataPtr &rdata);

private:

	bool config(const AosXmlTagPtr &config);
	bool start();
	bool retrieveAdditionalTransids();
	u64 getTransidLocked();
//	{
//		if (mStartTransid >= mEndTransid)
//		{
//			retrieveAdditionalTransids();
//		}
//		return mTransidPrefix + mStartTransid++;
//	}

	AosRemoteBkReqPtr getRequest(
						//const AosRemoteBkType::E type, 
						const OmnString &type,
						const u64 &transid,
						const AosXmlTagPtr &doc);

	AosRemoteBkReqPtr	getRequest(
						const OmnString &type,
						const u64 &transid,
						const u64 &iilid,
						const AosBuffPtr &buff,
						const u32 &siteid);

	AosRemoteBkReqPtr	getRequest(
						const OmnString &type,
						const u64 &transid,
						const u64 &iilid,
						const OmnString &word,
						const u32 &siteid);

	bool getConn();
	void checkDbRecordsLocked();
	bool processOneRecordLocked();
	bool addRequestLocked(
						const AosRemoteBkReqPtr &request, 
						const AosRundataPtr &rdata);
	bool saveReqToDbLocked(
						const AosRemoteBkReqPtr &request, 
						const AosRundataPtr &rdata);
	bool sendRequest(const OmnString &request);
	bool createDbTable();
};
#endif

