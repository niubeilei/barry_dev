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
// A Docid Bitmap is a bitmap where each bit in the bitmap maps to a 
// docid. In the current implementations, docid bitmap is a u64 array. 
// Its size is currently set to 500,000, which means that it can 
// handle 500,000 * 64 number of docids.  
//
// Modification History:
// 2011/02/19	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "Util/DocidBitmap.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"

u64 gAosDocidBitmapPtn[64] = 
{
	0x00000001,
	0x00000002,
	0x00000004,
	0x00000008,

	0x00000010,
	0x00000020,
	0x00000040,
	0x00000080,

	0x00000100,
	0x00000200,
	0x00000400,
	0x00000800,

	0x00001000,
	0x00002000,
	0x00004000,
	0x00008000,

	0x00010000,
	0x00020000,
	0x00040000,
	0x00080000,

	0x00100000,
	0x00200000,
	0x00400000,
	0x00800000,

	0x01000000,
	0x02000000,
	0x04000000,
	0x08000000,

	0x10000000,
	0x20000000,
	0x40000000,
	0x80000000
};


bool 
AosDocidBitmap::setBitmap(const u64 *docids, const u64 &num, u64 *bitmap)
{
	// This function sets the bitmap. 
	aos_assert_r(bitmap, false);
	aos_assert_r(docids, false);

	memset(bitmap, 0, eBitmapSize);
	if (num == 0) return true;

	for (u64 i=0; i<num; i++)
	{
		AOSBITMAP_SET(bitmap, docids[i]);
	}
	return true;
}


bool
AosDocidBitmap::merge(u64 *bitmap1, const u64 *bitmap2)
{
	// It ANDs the two bitmaps
	OmnNotImplementedYet;
	return false;
}


u32 
AosDocidBitmap::getDocs(
		u64 *docids, 
		const u64 *bitmap, 
		const int startidx, 
		const u32 pagesize)
{
	// This function retrieves up to 'pagesize' number of docs
	// from 'bitmap', starting at 'startidx'. Docids are stored
	// in 'docids'. It returns the number of docs retrieved.
	OmnNotImplementedYet;
	return 0;
}

