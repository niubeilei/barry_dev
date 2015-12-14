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
#ifndef AOS_ImageClient_ImageClient_h
#define AOS_ImageClient_ImageClient_h

#include "SEUtil/Ptrs.h"
#include "SingletonClass/SingletonObjId.h"
#include "SingletonClass/SingletonTplt.h"
#include "Thread/Ptrs.h"
#include "Thread/ThreadedObj.h"
#include "Util/Ptrs.h"
#include "Util/String.h"
#include "UtilComm/Ptrs.h"
#include "XmlInterface/XmlRc.h"
#include "XmlUtil/Ptrs.h"


OmnDefineSingletonClass(AosImageClientSingleton,
						AosImageClient,
						AosImageClientSelf,
						OmnSingletonObjId::eImageClient,
						"ImageClient");


class AosImageClient : virtual public OmnRCObject, 
				  virtual public OmnThreadedObj
{
	OmnDefineRCObject;

private:
	enum
	{
		eInitTransId = 100,
		eHeartbeatThrdId = 0,
		eReadThrdId = 1
	};

	OmnMutexPtr		mLock;
	OmnString		mRemoteAddr;
	int				mRemotePort;
	OmnTcpClientPtr	mConn;
	OmnThreadPtr    mHeartbeatThread;
	OmnThreadPtr    mReadThread;

public:
	AosImageClient();
	~AosImageClient();

    //
    // Singleton class interface
    //
    static AosImageClient*  	getSelf();
    virtual bool      	start();
    virtual bool        stop();
    virtual OmnString   getSysObjName() const {return "AosImageClient";}
    virtual OmnRslt     config(const OmnXmlParserPtr &def);
    virtual OmnSingletonObjId::E  
						getSysObjId() const 
						{
							return OmnSingletonObjId::eImageClient;
						}

	bool		start(const AosXmlTagPtr &config);

	// ThreadedObj Interface
	virtual bool    threadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread);
	virtual bool    signal(const int threadLogicId);
	virtual bool    checkThread(OmnString &err, const int thrdLogicId) const;

private:

	void 	sendRequest();
	void	connect();
	bool	reconnect();
	void 	sendHeartbeat();
	bool    heartbeatThrdFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread);
	bool    readThrdFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread);
	void 	msgReceived(const OmnConnBuffPtr &buff);
	bool	createFile(OmnString &path, const OmnConnBuffPtr buff);
	void	sendResponse(
			const AosXmlRc errcode,
			const OmnString &errmsg,
			const OmnString &contents);
};
#endif

