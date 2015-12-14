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
// 2013/01/06 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_BitmapUtil_BitmapStorageMgr_h
#define Aos_BitmapUtil_BitmapStorageMgr_h

#include "SEInterfaces/BitmapObj.h"
#include "SEInterfaces/BitmapStorageMgrObj.h"
#include "SingletonClass/SingletonObjId.h"
#include "SingletonClass/SingletonTplt.h"
#include "Util/Buff.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"

OmnDefineSingletonClass(AosBitmapStorageMgrSingleton,
						AosBitmapStorageMgr,
						AosBitmapStorageMgrSelf,
						OmnSingletonObjId::eBitmapStorageMgr,
						"BitmapStorageMgr");


class AosBitmapStorageMgr :virtual public AosBitmapStorageMgrObj 
{
	OmnDefineRCObject;
	typedef hash_map<const u64, u64, u64_hash, u64_cmp> uumap_t;
	typedef hash_map<const u64, u64, u64_hash, u64_cmp>::iterator uuitr_t;

	typedef hash_map<const u64, vector<u64>, u64_hash, u64_cmp> uuvmap_t;
	typedef hash_map<const u64, vector<u64>, u64_hash, u64_cmp>::iterator uuvitr_t;

	typedef hash_map<const u64, AosBuffPtr, u64_hash, u64_cmp> sidbfmap_t;
	typedef hash_map<const u64, AosBuffPtr, u64_hash, u64_cmp>::iterator sidbfitr_t;

	typedef hash_map<const u32, AosDocFileMgrObjPtr, u32_hash, u32_cmp> dmap_t;
	typedef hash_map<const u32, AosDocFileMgrObjPtr, u32_hash, u32_cmp>::iterator dmapitr_t;

//	typedef hash_map<const u64, AosLevelOneNodePtr, u64_hash, u64_cmp> nmap_t;
//	typedef hash_map<const u64, AosLevelOneNodePtr, u64_hash, u64_cmp>::iterator nmapitr_t;
private:
	enum
	{
		eHeaderRecordSize = sizeof(u64),
		eDftNumBitmapsPerDoc = 30,

		eBitmapDocidBlockSize = 1000,
		eBitmapDocidInitValue = 1000000,
		eBitmapMaxDocid = 0x00ffffffffffffffULL,
		eBitmapDocidPrefix = 0,
		eMaxReusedDocids = 1000000,
		eLONMapCacheSize = 100
	};

public:
	AosBitmapStorageMgr();
	~AosBitmapStorageMgr();
	static AosBitmapStorageMgr*	getSelf();
	AosBitmapObjPtr retrieveLeafAsBitmap(
					const AosRundataPtr &rdata, 
					const u64 node_id, 
					const u64 parent_id); 
};


#endif

