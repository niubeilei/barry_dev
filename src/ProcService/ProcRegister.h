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
#ifndef AOS_ProcRegister_ProcRegister_h
#define AOS_ProcRegister_ProcRegister_h

#include "SingletonClass/SingletonObjId.h"
#include "SingletonClass/SingletonTplt.h"
#include "Thread/ThreadedObj.h"
#include "Thread/Ptrs.h"
#include "XmlUtil/XmlTag.h"
#include "ProcServer/Ptrs.h"
#include "Rundata/Rundata.h"
#include "Rundata/Ptrs.h"
#include "ProcUtil/ManagedServer.h"

#include <vector>
using namespace std;

OmnDefineSingletonClass(AosProcRegisterSingleton,
						AosProcRegister,
						AosProcRegisterSelf,
						OmnSingletonObjId::eProcRegister,
						"ProcRegister");


class AosProcRegister : virtual public OmnThreadedObj
{
	OmnDefineRCObject;

private:
	OmnThreadPtr        mThread;
	OmnMutexPtr         mLock;
	AosRundataPtr		mRundata;
	AosXmlTagPtr		mConfig;

	vector<AosManagedServerPtr>	mServers;

public:
	AosProcRegister();
	~AosProcRegister();

    // OmnThreadedObj Interface
    virtual bool    threadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread);

    // Singleton class interface
    static AosProcRegister *    	getSelf();
    virtual bool      	start();
    virtual bool        stop();
	bool signal(const int threadLogicId);
	bool checkThread(OmnString &err, const int thrdLogicId) const;
    virtual OmnString   getSysObjName() const {return "AosProcRegister";}
    virtual OmnSingletonObjId::E  
						getSysObjId() const 
						{
							return OmnSingletonObjId::eProcRegister;
						}
    virtual bool config(const AosXmlTagPtr &def);
	bool 	start(const AosXmlTagPtr &config);
	bool    registerToServer(
				const OmnString &req,
				OmnString &resp,
				OmnString &errmsg);
	bool	initServers();
	void	prepareReq(OmnString &req);
};
#endif
