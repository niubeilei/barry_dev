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
// 	Created: 12/13/2009 by Chen Ding
////////////////////////////////////////////////////////////////////////////
#if 0
#ifndef AOS_IILMgr_IILHelper_h
#define AOS_IILMgr_IILHelper_h

#include "SingletonClass/SingletonObjId.h"
#include "SingletonClass/SingletonTplt.h"
#include "Thread/Ptrs.h"
#include "Thread/Mutex.h"
#include "Thread/ThreadedObj.h"
#include "SEUtil/Ptrs.h"
#include "SEUtil/DocTags.h"
#include "Util/Opr.h"
#include "SearchEngine/Ptrs.h"
#include "SEUtil/SeTypes.h"
#include "IdGen/U64IdGen.h"
#include "Util/Ptrs.h"
#include "Util/String.h"
#include "Util/DynArray.h"
#include "UtilComm/Ptrs.h"
#include "XmlUtil/XmlTag.h"

OmnDefineSingletonClass(AosIILHelperSingleton,
						AosIILHelper,
						AosIILHelperSelf,
						OmnSingletonObjId::eIILHelper,
						"IILHelper");


class AosIILHelper : virtual public OmnRCObject, 
				  virtual public OmnThreadedObj
{
	OmnDefineRCObject;

public:
	AosIILHelper();
	~AosIILHelper();

    // Singleton class interface
    static AosIILHelper*  	getSelf();
    virtual bool      	start();
    virtual bool        stop();
    virtual bool		config(const AosXmlTagPtr &def);

	// ThreadedObj Interface
	virtual bool    threadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread);
	virtual bool    signal(const int threadLogicId);
	virtual bool    checkThread(OmnString &err, const int thrdLogicId) const;

	bool	
	addWhoVisited(
		const OmnString &siteid,
		const AosXmlTagPtr &doc, 
		const OmnString &cid,
		const AosRundataPtr &rdata);
};
#endif
#endif
