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
//
// Modification History:
// 2013/05/02 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SEInterfaces_BitmapMgrObj_h
#define Aos_SEInterfaces_BitmapMgrObj_h

#include "Rundata/Ptrs.h"
#include "SEInterfaces/Ptrs.h"
#include "SEInterfaces/BitmapObj.h"
#include "Util/Buff.h"
#include "Util/Ptrs.h"
#include "Util/RCObject.h"
#include "Util/Ptrs.h"


class AosBitmapMgrObj : public OmnRCObject
{
	static AosBitmapMgrObjPtr	smObject;

public:
	static bool setBitmapMgr(const AosBitmapMgrObjPtr &obj);
	static AosBitmapMgrObjPtr getBitmapMgr();

	virtual u64 nextBitmapId(
						const AosRundataPtr &rdata,
						const u64 iilid, 
						const int node_level) = 0;

	virtual AosBitmapObjPtr getBitmap() = 0;
	virtual void returnBitmap(const AosBitmapObjPtr &bitmap) = 0;

	static AosBitmapObjPtr getBitmapStatic();
	static void returnBitmapStatic(const AosBitmapObjPtr &bitmap);

};
#endif



