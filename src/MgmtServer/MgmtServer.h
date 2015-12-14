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
// 12/05/2011	Created by Linda 
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_MgmtServer_MgmtServer_h
#define AOS_MgmtServer_MgmtServer_h

#include "SingletonClass/SingletonObjId.h"
#include "SingletonClass/SingletonTplt.h"
#include "Thread/ThreadedObj.h"
#include "Thread/Ptrs.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "UtilComm/TcpListener.h"



OmnDefineSingletonClass(AosMgmtServerSingleton,
                        AosMgmtServer,
                        AosMgmtServerSelf,
                        OmnSingletonObjId::eMgmtServer,
                        "MgmtServer");



class AosMgmtServer : public OmnThreadedObj
{
	OmnDefineRCObject;
	
private:
	enum
	{
		eMaxDevices = 60000
	};

	int						mDeviceId;
	OmnMutexPtr				mLock;
	OmnCondVarPtr			mCondVar;
	AosDevicePtr		   	mDevices[eMaxDevices];

public:
	AosMgmtServer(const int device_id);
	~AosMgmtServer();

	// Singleton interface
    static AosDocClient*    getSelf();
    virtual bool        	start();
    virtual bool        	stop();
    virtual bool			config(const AosXmlTagPtr &def);

	// ThreadedObj interface
	virtual bool	threadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread);
	virtual bool	signal(const int threadLogicId);
    virtual bool    checkThread(OmnString &err, const int thrdLogicId) const;

	// TcpListener Interface
	virtual OmnString	getTcpListenerName() const;
	virtual void		msgRecved(const OmnConnBuffPtr &buff,
								  const OmnTcpClientPtr &conn);
	virtual void		connCreated(const OmnTcpClientPtr &conn);
	virtual void		connClosed(const OmnTcpClientPtr &client);
};

#endif

