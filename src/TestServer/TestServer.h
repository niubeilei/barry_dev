////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: TestServer.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_TestServer_TestServer_h
#define Omn_TestServer_TestServer_h

#include "Message/Ptrs.h"
#include "SingletonClass/SingletonTplt.h"
#include "TestServer/Ptrs.h"
#include "TestUtil/Ptrs.h"
#include "Thread/Ptrs.h"
#include "Util/IpAddr.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/ValList.h"
#include "UtilComm/Ptrs.h"
#include "UtilComm/TcpListener.h"


OmnDefineSingletonClass(OmnTestServerSingleton,
                        OmnTestServer,
                        OmnTestServerSelf,
                        OmnSingletonObjId::eTestServer,
                        "TestServer");


class OmnTestServer : public OmnTcpListener
{
	OmnDefineRCObject;

private:
	OmnMutexPtr			mLock;
	OmnTcpCommSvrPtr	mComm;
//	OmnThreadPtr		mThread;
	OmnIpAddr			mLocalAddr;
	int					mLocalPort;
	OmnIpAddr			mRemoteAddr;
	int					mRemotePort;
	OmnVList<OmnTestAgentInfoPtr>	mAgentList;
	OmnVList<OmnRpcCallerPtr>		mRpcQueue;
	
public:
	OmnTestServer();
	virtual ~OmnTestServer();

    //
    // OmnThreadedObj interface
    //
//	virtual bool threadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread);
//	virtual bool signal(const int threadLogicId);
//	virtual bool checkThread(OmnString &errmsg, const int tid) const;

    //  
    // Singleton Interface
    //
    static OmnTestServer *  getSelf();
    virtual bool    start();
    virtual bool    stop(); 
    virtual OmnRslt config(const OmnXmlParserPtr &parser);

	// 
	// TcpListener interface
	//
	virtual OmnString	getTcpListenerName() const {return "TestServer";}
	virtual void		msgRecved(const OmnConnBuffPtr &buff,
								  const OmnTcpClientPtr &conn);

	bool				callFunc(const OmnRpcCallerPtr &rpc);
	OmnTestAgentInfoPtr	getAgent(const OmnString &name);

private:
	bool		procBuff(const OmnConnBuffPtr &buff);
	bool		procRemoteProcCallResp(const OmnSmRPCRespPtr msg);
	bool		procAgentRegistration(const OmnSmTestAgentRegPtr &msg, 
						const OmnTcpClientPtr &client);

};
#endif

