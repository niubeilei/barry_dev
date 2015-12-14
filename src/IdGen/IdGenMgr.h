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
#ifndef AOS_IdGen_IdGenMgr_h
#define AOS_IdGen_IdGenMgr_h

#include "IdGen/Ptrs.h"
#include "Thread/Ptrs.h"
#include "Rundata/Ptrs.h"
#include "SingletonClass/SingletonObjId.h"
#include "SingletonClass/SingletonTplt.h"
#include "SEUtil/Ptrs.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"
#include "Util/Ptrs.h"
#include "Util/FileWBack.h"
#include "XmlInterface/Server/Ptrs.h"
#include "XmlInterface/Server/SyncServerReqCb.h"

OmnDefineSingletonClass(AosIdGenMgrSingleton,
	AosIdGenMgr,
	AosIdGenMgrSelf,
	OmnSingletonObjId::eIdGenMgr,
	"IdGenMgr");


class AosIdGenMgr : virtual public OmnRCObject
{
	OmnDefineRCObject;

private:
	enum
	{
		eMaxIdGens = 100
	};

	OmnMutexPtr		mLock;
	AosIdGenDefPtr	mDefs[eMaxIdGens];
	int				mNumDefs;
	OmnString		mFname;
	AosFileWBackPtr	mFile;

public:
	AosIdGenMgr();
	~AosIdGenMgr();

	// Singleton class interface
	static AosIdGenMgr*   getSelf();
	virtual bool        start();
	virtual bool        stop();
	virtual bool		config(const AosXmlTagPtr &def);

	AosIdGenDefPtr 		getIdDef(const OmnString &name);
	AosIdGenDefPtr 		getIdDef(const AosXmlTagPtr &def);
	bool 				initIdGens();
	AosFileWBackPtr		getFile() const {return mFile;}

	bool getNextId( 	const OmnString &idgen_objid, 
						u64 &next_id,
						const u64 &init_value,
						const AosRundataPtr &rdata);
	AosXmlTagPtr createIdGen(
						const OmnString &objid, 
						const bool is_public,
						const u64 &init_value,
						const int blocksize, 
						const AosRundataPtr &rdata);

private:
	AosIdGenDefPtr retrieveDefault(const OmnString &name, const int idx);
};

#endif
