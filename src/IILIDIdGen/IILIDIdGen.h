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
#ifndef AOS_IILIDIdGen_IILIDIdGen_h
#define AOS_IILIDIdGen_IILIDIdGen_h

#include "Debug/Debug.h"
#include "IILIDIdGen/Ptrs.h"
#include "Rundata/Ptrs.h"
#include "SEUtil/Ptrs.h"
#include "SingletonClass/SingletonObjId.h"
#include "SingletonClass/SingletonTplt.h" 
#include "Thread/Ptrs.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"
#include "Util/Ptrs.h"
#include "XmlUtil/Ptrs.h"

OmnDefineSingletonClass(AosIILIDIdGenSingleton,
						AosIILIDIdGen,
						AosIILIDIdGenSelf,
						OmnSingletonObjId::eIILIDIdGen,
						"IILIDIdGen");


class AosIILIDIdGen : virtual public OmnRCObject
{
	OmnDefineRCObject;

private:
	enum
	{
		eDftBatchSize = 100
	};

	OmnMutexPtr				mLock;
	//AosU64IdGenForIILPtr	*mIILIdGens;
	map<u32, AosU64IdGenForIILPtr>  mIILIdGens;		// Ketty 2013/02/27
	u32						mNumVirtuals;
	int						mBatchSize;

	// Ketty 2013/02/27
	u64         mCrtid;
	u64         mMaxid;
	u32         mBlocksize;
	
public:
	AosIILIDIdGen();
	~AosIILIDIdGen();

	// Singleton class interface
	static AosIILIDIdGen*   getSelf();
	virtual bool        	start();
	virtual bool        	stop();
	virtual bool			config(const AosXmlTagPtr &def);
	
	u64		nextIILId(const u32 vid, const AosRundataPtr &rdata);
	//bool	createIdGens(const AosXmlTagPtr &defs, const bool create_flag);
	
	// Ketty 2013/02/27
	AosU64IdGenForIILPtr    getIILIdGen(const int virtual_id, const AosRundataPtr &rdata);
};

#endif
