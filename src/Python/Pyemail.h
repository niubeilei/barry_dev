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
// This file is generated automatically by the ProgramAid facility.   
//
// Modification History:
// 3/23/2007: Created by cding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_Python_Pyemail_h
#define Aos_Python_Pyemail_h

#include "aosUtil/Types.h"
#include "Obj/ObjDb.h"
#include "Rundata/Ptrs.h"
#include "SEUtil/Ptrs.h"
#include "SingletonClass/SingletonObjId.h"
#include "SingletonClass/SingletonTplt.h"
#include "Thread/Mutex.h"
#include "Thread/ThreadedObj.h"
#include "Thread/Thread.h"
#include "Thread/Ptrs.h"
#include "Util/RCObjImp.h"
#include "UserMgmt/Ptrs.h"
#include "Util/String.h"
#include "Util/StrParser.h"
#include "XmlInterface/XmlRc.h"
#include "XmlUtil/Ptrs.h"

#include <queue>

OmnDefineSingletonClass(AosPyemailSingleton,
						AosPyemail,
						AosPyemailSelf,
						OmnSingletonObjId::ePyemail,
						"Pyemail");

struct AosStRequest : public OmnRCObject
{
private:
	OmnDefineRCObject;

public:
	OmnString sender;
	OmnString sender_passwd;
	OmnString receiver;
	OmnString subject;
	OmnString contents;
	OmnString server;
	OmnString count;
	AosRundataPtr rdata;
};

class AosPyemail : virtual public OmnThreadedObj 
{
	OmnDefineRCObject;

private:
	OmnMutexPtr         mLock;
	OmnCondVarPtr       mCondVar;
	queue<AosStRequestPtr> mQueue;
	int 				mNumReqs;
	OmnThreadPtr        mThread;

public:

    AosPyemail();
    ~AosPyemail();
	// Singleton class interface
	static AosPyemail*    getSelf();
	virtual bool          start();
	virtual bool          stop();
	virtual bool          config(const AosXmlTagPtr &def);

	//ThreadedObj Interface
	virtual bool    threadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread);
	virtual bool    signal(const int threadLogicId);
	virtual bool    checkThread(OmnString &err, const int thrdLogicId) const;

	bool	sendmail(AosXmlTagPtr &childelem, const AosRundataPtr &rdata);
	static void serverInit();

private:

	void	addRequest(
				const OmnString &sender,
				const OmnString &senderpasswd,
				const OmnString &receiver,
				const OmnString &subject,
				const OmnString &body,
				const OmnString &server,
				const OmnString &count,
				const AosRundataPtr &rdata);

	AosStRequestPtr	getRequest();
};
#endif

