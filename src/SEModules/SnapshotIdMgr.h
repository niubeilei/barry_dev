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
// 	Created: 07/29/2010 by James Kong
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_SEModules_SnapshotIdMgr_h
#define AOS_SEModules_SnapshotIdMgr_h

#include "Rundata/Ptrs.h"
#include "SearchEngine/Ptrs.h"
#include "SEUtil/Ptrs.h"
#include "SEUtil/DocTags.h"
#include "SEUtil/SeTypes.h"
#include "SingletonClass/SingletonObjId.h"
#include "SingletonClass/SingletonTplt.h"
#include "Security/Ptrs.h"
#include "Thread/Ptrs.h"
#include "Thread/ThreadedObj.h"
#include "Util/Ptrs.h"
#include "Util/String.h"
#include "UtilComm/Ptrs.h"
#include "XmlUtil/Ptrs.h"
#include "XmlInterface/XmlRc.h"
#include <vector>
using namespace std;



OmnDefineSingletonClass(AosSnapshotIdMgrSingleton,
						AosSnapshotIdMgr,
						AosSnapshotIdMgrSelf,
						OmnSingletonObjId::eSnapshotIdMgr,
						"SnapshotIdMgr");


class AosSnapshotIdMgr : virtual public OmnRCObject
{
	OmnDefineRCObject;

private:

public:
	AosSnapshotIdMgr();
	~AosSnapshotIdMgr();

    // Singleton class interface
    static AosSnapshotIdMgr*  	getSelf();
    virtual bool      	start();
    virtual bool        stop();
    virtual bool		config(const AosXmlTagPtr &def);
	
	u64 createSnapshotId(const AosRundataPtr &rdata);
	bool deleteSnapshotId(const u64 snap_id, const AosRundataPtr &rdata);
};
#endif

