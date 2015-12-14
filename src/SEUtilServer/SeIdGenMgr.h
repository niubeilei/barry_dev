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
// 10/04/2009	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_SEUtilServer_SeIdGenMgr_h
#define AOS_SEUtilServer_SeIdGenMgr_h

#include "Debug/Debug.h"
#include "IdGen/Ptrs.h"
#include "IdGen/U64IdGenWithIIL.h"
#include "Rundata/Ptrs.h"
#include "SingletonClass/SingletonObjId.h"
#include "SingletonClass/SingletonTplt.h"
#include "SEUtil/Ptrs.h"
#include "Thread/Ptrs.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"
#include "Util/Ptrs.h"
#include "XmlUtil/Ptrs.h"
#include "XmlInterface/Server/Ptrs.h"
#include "XmlInterface/Server/SyncServerReqCb.h"

OmnDefineSingletonClass(AosSeIdGenMgrSingleton,
	AosSeIdGenMgr,
	AosSeIdGenMgrSelf,
	OmnSingletonObjId::eSeIdGenMgr,
	"SeIdGenMgr");

#define AOSIDGEN_DOCID				"docid"
#define AOSIDGEN_IILID				"iilid"
#define AOSIDGEN_IMAGEID			"imageid"
#define AOSIDGEN_COMPID				"compid"
#define AOSIDGEN_SSID				"ssid"
#define AOSIDGEN_CLOUDID			"cloudid"

#define AOSCIDPREFIX 				""

class AosSeIdGenMgr : virtual public OmnRCObject
{
	OmnDefineRCObject;

private:
	enum
	{
		eInitCloudid = 2000000,
		eInitCompId = 1000000,
		eInitSsid = 1000000

	};

	bool					mStarted;
	OmnMutexPtr				mLock;
	AosU64IdGenWithIILPtr	mCloudidGen;
	AosU64IdGenWithIILPtr	mCompIdGen;
	AosU64IdGenWithIILPtr	mSsidGen;

public:
	AosSeIdGenMgr();
	~AosSeIdGenMgr();

	// Singleton class interface
	static AosSeIdGenMgr*   getSelf();
	virtual bool        start();
	virtual bool        stop();
	virtual bool		config(const AosXmlTagPtr &def) {return true;}

	u64	nextSsid(const AosRundataPtr &rdata);
	OmnString nextCid(const u32 siteid, const AosRundataPtr &rdata);
	u32 nextCompIdBlock();
};

#endif
