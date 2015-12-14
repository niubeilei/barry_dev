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
// 09/27/2010	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SEUtilSv_UserDocMgr_h
#define Aos_SEUtilSv_UserDocMgr_h

#include "Rundata/Ptrs.h"
#include "SearchEngine/Ptrs.h"
#include "SEUtil/Ptrs.h"
#include "SEUtil/SeTypes.h"
#include "SEUtil/DocTags.h"
#include "SingletonClass/SingletonObjId.h"
#include "SingletonClass/SingletonTplt.h"
#include "Thread/Ptrs.h"
#include "Thread/Mutex.h"
#include "Thread/ThreadedObj.h"
#include "Util/Ptrs.h"
#include "Util/String.h"
#include "Util/DynArray.h"
#include "UtilComm/Ptrs.h"
#include "XmlUtil/Ptrs.h"


OmnDefineSingletonClass(AosUserDocMgrSingleton,
						AosUserDocMgr,
						AosUserDocMgrSelf,
						OmnSingletonObjId::eIILHelper,
						"IILHelper");


class AosUserDocMgr : virtual public OmnRCObject, 
				  	 virtual public OmnThreadedObj
{

	OmnDefineRCObject;

private:

public:
	AosUserDocMgr();
	~AosUserDocMgr();

    //
    // Singleton class interface
    //
    static AosUserDocMgr*  	getSelf();
    virtual bool      	start();
    virtual bool        stop();
    virtual bool		config(const AosXmlTagPtr &def){return true;}

	// ThreadedObj Interface
	virtual bool    threadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread);
	virtual bool    signal(const int threadLogicId);
	virtual bool    checkThread(OmnString &err, const int thrdLogicId) const;

	AosXmlTagPtr getUserDoc(const u64 &docid, const AosRundataPtr &rdata);
	AosXmlTagPtr getUserDoc(const u32 siteid, const OmnString &cid, const AosRundataPtr &rdata);
};
#endif
