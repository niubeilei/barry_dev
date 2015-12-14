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
// 06/16/2011	Created by Jozhi Peng
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_MsgClient_MsgClient_h
#define AOS_MsgClient_MsgClient_h

#include "SingletonClass/SingletonImpl.cpp"
#include "SEInterfaces/MsgClientObj.h"
#include "Rundata/Rundata.h"


OmnDefineSingletonClass(AosMsgClientSingleton,
                        AosMsgClient,
                        AosMsgClientSelf,
                        OmnSingletonObjId::eMsgClient,
                        "MsgClient");

class AosMsgClient : public OmnThreadedObj,
					 public AosMsgClientObj
{
	OmnDefineRCObject;
private:
	struct RequestInfo
	{
		OmnString 			mRequest;
		AosRundataPtr		mRundata;
	};

private:
	OmnMutexPtr					mLock;
	OmnCondVarPtr				mCondVar;
	OmnThreadPtr                mThread;
	vector<OmnString>    		mMsgSvrUrl;
	deque<u32>					mWaitProcs;
	deque<RequestInfo>			mWaitRequest;
	bool						mIsStart;

public:
	AosMsgClient();
	~AosMsgClient();

	//
    // Singleton class interface
    //
    static AosMsgClient*    getSelf();
    virtual bool        start();
    virtual bool        stop();
	virtual bool        config(const AosXmlTagPtr &config);

	virtual bool    threadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread);
	virtual bool    signal(const int threadLogicId);
	bool start(const AosXmlTagPtr &config);

	virtual bool procMsgSvrRequest(
				const OmnString &request,
				const AosRundataPtr &rdata);

	virtual OmnString getMsgSvrUrl();

	virtual bool startProcCb(const u32 logic_pid);
	virtual bool stopProcCb(const u32 logic_pid);
};
#endif

