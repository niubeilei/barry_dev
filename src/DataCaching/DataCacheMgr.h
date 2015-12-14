////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Cacheering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Cacheering, Inc. or its derivatives
//
// Modification History:
// 2013/01/27 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_DataCaching_DataCacheMgr_h
#define AOS_DataCaching_DataCacheMgr_h

#include "Bitmap/Ptrs.h"
#include "Rundata/Ptrs.h"
#include "SingletonClass/SingletonObjId.h"
#include "SingletonClass/SingletonTplt.h"
#include "TransServer/TransProc.h"
#include <queue>
using namespace std;

OmnDefineSingletonClass(AosDataCacheMgrSingleton,
						AosDataCacheMgr,
						AosDataCacheMgrSelf,
						OmnSingletonObjId::eDataCacheMgr,
						"DataCacheMgr");

class AosDataCacheMgr : public OmnRCObject
{
	OmnDefineRCObject;

private:

public:
	AosDataCacheMgr();
	~AosDataCacheMgr();

};
#endif

