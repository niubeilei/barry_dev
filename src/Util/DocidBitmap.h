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
// 2011/02/19	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Omn_Util_DocidBitmap_h
#define Omn_Util_DocidBitmap_h

#include "aosUtil/Types.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"

#define AOSBITMAP_SET(bitmap, docid) (bitmap)[(docid)>>6] |= gAosDocidBitmapPtn[(docid)&0x3f] 
#define AOSBITMAP_CHECK(bitmap, docid) (bitmap)[(docid)>>6] & gAosDocidBitmapPtn[(docid)&0x3f]
#define AOSBITMAP_RESET(bitmap) memset((bitmap), 0, AosDocidBitmap::eBitmapSize)

class AosDocidBitmap : public virtual OmnRCObject
{
	OmnDefineRCObject;

public:
	enum
	{
		eBitmapSize = 500000
	};

public:
	AosDocidBitmap(){}
	~AosDocidBitmap() {}

	static bool setBitmap(const u64 *docids, const u64 &num, u64 *bitmap);
	static bool	merge(u64 *bitmap1, const u64 *bitmap2);
	static u32	getDocs(u64 *docids, 
					const u64 *bitmap,
					const int startidx,
					const u32 pagesize);
};
#endif
