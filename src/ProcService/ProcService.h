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
// 09/26/2009	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_ProcService_ProcService_h
#define AOS_ProcService_ProcService_h

#include "SingletonClass/SingletonObjId.h"
#include "SingletonClass/SingletonTplt.h"
#include "Thread/ThreadedObj.h"
#include "Thread/Ptrs.h"
#include "XmlUtil/XmlTag.h"
#include "ProcServer/Ptrs.h"
#include "Rundata/Rundata.h"
#include "Rundata/Ptrs.h"
#include "ProcUtil/ManagedServer.h"
#include "UtilComm/TcpServer.h"
#include "UtilComm/TcpListener.h"

#include <vector>
using namespace std;

OmnDefineSingletonClass(AosProcServiceSingleton,
						AosProcService,
						AosProcServiceSelf,
						OmnSingletonObjId::eProcService,
						"ProcService");


class AosProcService : virtual public OmnThreadedObj,
					   virtual public OmnTcpListener
{
	OmnDefineRCObject;

private:
	OmnThreadPtr        mThread;
	OmnMutexPtr         mLock;
	AosRundataPtr		mRundata;
	AosXmlTagPtr		mConfig;
	OmnString			mAddr;
	int					mPort;
	OmnTcpServerPtr		mServer;

public:
	AosProcService();
	~AosProcService();

	virtual void 		connCreated(const OmnTcpClientPtr &conn){}
	virtual void 		connClosed(const OmnTcpClientPtr &conn){}
	virtual OmnString   getTcpListenerName() const {return "ProcService";};
    // OmnThreadedObj Interface
    virtual bool    threadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread);

    // Singleton class interface
    static AosProcService *    	getSelf();
    virtual bool      	start();
    virtual bool        stop();
	bool signal(const int threadLogicId);
	bool checkThread(OmnString &err, const int thrdLogicId) const;
    virtual OmnString   getSysObjName() const {return "AosProcService";}
    virtual OmnSingletonObjId::E  
						getSysObjId() const 
						{
							return OmnSingletonObjId::eProcService;
						}
    virtual bool 	config(const AosXmlTagPtr &def);
	bool 	start(const AosXmlTagPtr &config);
	virtual void  msgRecved(const OmnConnBuffPtr &buff, const OmnTcpClientPtr &conn);
	bool	doComand(AosXmlTagPtr &cmd, OmnString &resp);
	void	startListening();

};
#endif
