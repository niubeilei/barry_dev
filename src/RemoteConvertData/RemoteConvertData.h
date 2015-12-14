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
// 06/16/2011	Created by Jozhi Peng
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_RemoteConvertData_RemoteConvertData_h
#define AOS_RemoteConvertData_RemoteConvertData_h

#include "Rundata/Rundata.h"
#include "SEInterfaces/Ptrs.h"
#include "SEClient/SEClient.h"
#include "SEClient/Ptrs.h"

#include <queue>

OmnDefineSingletonClass(AosRemoteConvertDataSingleton,
						AosRemoteConvertData,
						AosRemoteConvertDataSelf,
						OmnSingletonObjId::eRemoteConvertData,
						"RemoteConvertData");

class AosRemoteConvertData : virtual public OmnRCObject,
							virtual public OmnThreadedObj
{
	OmnDefineRCObject;
public:
	enum
	{
		eMaxThreads = 10
	};

	enum
	{
		eInitTransId = 100
	};

	enum ReqType
	{
		eCreateDoc,
		eCreateArcd
	};

	struct ReqStruct
	{
		ReqType			mType;
		u32				mSiteid;
		AosXmlTagPtr	mDoc;
	};

private:
	static AosDocClientObjPtr	smDocClient;
	static AosIILClientObjPtr	smIILClient;

	OmnMutexPtr     			mLock;
	OmnString       			mRemoteAddr;
	int             			mRemotePort;
	AosSEClientPtr  			mConn;
	u32             			mTransId;
	OmnThreadPtr        		mThreads[eMaxThreads];
	queue<ReqStruct>			mRequests;
	queue<ReqStruct>			mRequests1;
	OmnCondVarPtr       		mCondVar;

public:
	AosRemoteConvertData();
	~AosRemoteConvertData();

    // OmnThreadedObj Interface
    virtual bool    threadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread);
    virtual bool    signal(const int threadLogicId);
    virtual bool    checkThread(OmnString &err, const int thrdLogicId) const;

    // Singleton class interface
    static AosRemoteConvertData *	getSelf();
    virtual bool      	start();
    virtual bool        stop();
	virtual bool        config(const AosXmlTagPtr &def);

	bool addReq(const ReqType type,
			const u32 siteid,
			const AosXmlTagPtr &doc);

	bool sendCreateObjReq(
			const u32 siteid,
			const char *doc);

	bool ModifyAccessRcd(
			const u32 siteid,
			const AosXmlTagPtr &doc,
			const u64 &owndocid,
			const OmnString &ssid,
			const u64 &urldocid);

	bool sendServerReq(
			const u32 siteid,
			const OmnString &reqid,
			const OmnString &args,
			const AosXmlTagPtr &obj);

	bool rebuildDb(
			const u64 &startDocid,
			const u64 &maxDocid,
			const AosRundataPtr &rdata);

	bool cleanData(
			const AosXmlTagPtr &doc);

	bool convertDataForRebuild(
			const AosXmlTagPtr &doc);
};
#endif
