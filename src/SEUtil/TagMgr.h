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
// 	09/14/2010 	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_SEUtil_TagMgr_h
#define AOS_SEUtil_TagMgr_h

#include "SearchEngine/Ptrs.h"
#include "SEUtil/Ptrs.h"
#include "SEUtil/DocTags.h"
#include "SingletonClass/SingletonObjId.h"
#include "SingletonClass/SingletonTplt.h"
#include "Thread/Ptrs.h"
#include "Thread/ThreadedObj.h"
#include "Util/Ptrs.h"
#include "Util/String.h"
#include "UtilComm/Ptrs.h"
#include "XmlUtil/Ptrs.h"
#include "XmlInterface/XmlRc.h"


OmnDefineSingletonClass(AosTagMgrSingleton,
						AosTagMgr,
						AosTagMgrSelf,
						OmnSingletonObjId::eTagMgr,
						"TagMgr");


class AosTagMgr : virtual public OmnRCObject
{
	OmnDefineRCObject;

public:

private:
	bool		mIsStopping;

public:
	AosTagMgr();
	~AosTagMgr();

    //
    // Singleton class interface
    //
    static AosTagMgr*  	getSelf();
    virtual bool      	start();
    virtual bool        stop();
    virtual bool		config(const AosXmlTagPtr &def);

	bool	setSysTags(const AosXmlTagPtr &doc);
};
#endif

