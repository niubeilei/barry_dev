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
#ifndef AOS_ImageBack_ImageBack_h
#define AOS_ImageBack_ImageBack_h

#include "SEUtil/Ptrs.h"
#include "SingletonClass/SingletonObjId.h"
#include "SingletonClass/SingletonTplt.h"
#include "Thread/Ptrs.h"
#include "Thread/ThreadedObj.h"
#include "Util/Ptrs.h"
#include "Util/String.h"
#include "UtilComm/Ptrs.h"
#include "XmlInterface/XmlRc.h"


OmnDefineSingletonClass(AosImageBackSingleton,
						AosImageBack,
						AosImageBackSelf,
						OmnSingletonObjId::eImageBack,
						"ImageBack");


class AosImageBack : virtual public OmnRCObject, 
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
	OmnString		mLocalAddr;
	int				mLocalPort;
	int				mNumPorts;
	OmnThreadPtr    mThread;
	deque<AosImageReqPtr>	mQueue;
	OmnTcpClientPtr	mConns;

public:
	AosImageBack();
	~AosImageBack();

    //
    // Singleton class interface
    //
    static AosImageBack*  	getSelf();
    virtual bool      	start();
    virtual bool        stop();
    virtual OmnString   getSysObjName() const {return "AosImageBack";}
    virtual OmnRslt     config(const OmnXmlParserPtr &def);
    virtual OmnSingletonObjId::E  
						getSysObjId() const 
						{
							return OmnSingletonObjId::eImageBack;
						}

	bool		start(const AosXmlTagPtr &config);

	// ThreadedObj Interface
	virtual bool    threadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread);
	virtual bool    signal(const int threadLogicId);
	virtual bool    checkThread(OmnString &err, const int thrdLogicId) const;

	bool	addReq(const OmnString &fname);

private:
};
#endif

