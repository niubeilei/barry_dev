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
// 2013/03/06 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_SeServerUdp_SeServerUdp_h
#define AOS_SeServerUdp_SeServerUdp_h

#include "Rundata/Ptrs.h"
#include "SingletonClass/SingletonObjId.h"
#include "SingletonClass/SingletonTplt.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "UtilComm/Ptrs.h"
#include "UtilComm/CommListener.h"
#include <queue>
using namespace std;

OmnDefineSingletonClass(AosSeServerUdpSingleton,
						AosSeServerUdp,
						AosSeServerUdpSelf,
						OmnSingletonObjId::eSeServerUdp,
						"SeServerUdp");

class AosSeServerUdp : public OmnThreadedObj,
					   public OmnCommListener
{
	OmnDefineRCObject;

private:
	enum
	{
		eMinQueueSize = 100,
		eMaxQueueSize = 1000000,
		eDftQueueSize = 10000
	};

	OmnMutexPtr				mLock;
	OmnUdpCommPtr			mComm;
	int						mReqHead;
	int						mProcIdx;
	vector<AosSeReqProcPtr>	mRequests;

public:
	AosSeServerUdp();
	~AosSeServerUdp();

    // Singleton class interface
    static AosSeServerUdp *    	getSelf();
    virtual bool      	start();
    virtual bool        stop();
    virtual bool		config(const AosXmlTagPtr &def);

	// OmnThreadedObj interface
	virtual bool threadFunc(OmnThrdStatus::E &, const OmnThreadPtr &);
	virtual bool signal(const int threadLogicId);
	virtual bool checkThread(OmnString &errmsg, const int tid) const;
	
	// OmnCommListener Interface
	virtual bool		msgRead(const OmnConnBuffPtr &buff);
	virtual OmnString	getCommListenerName() const;
	virtual void 		readingFailed();

private:
};
#endif

