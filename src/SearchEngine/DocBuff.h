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
// 10/28/2009	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_SearchEngine_DocBuff_h
#define AOS_SearchEngine_DocBuff_h

#include "Util/RCObject.h"
#include "Util/RCObjImp.h"


struct AosDocBuff : virtual public OmnRCObject
{
	OmnDefineRCObject;

	char *		mBuff;
	u32			mMemSize;
	u32			mDataLen;

	AosDocBuff()
	:
	mBuff(0),
	mMemSize(0),
	mDataLen(0)
	{
	}

	~AosDocBuff()
	{
		if (mBuff) OmnDelete [] mBuff;
		mBuff = 0;
		mDataLen = 0;
	}

	char *	getBuff() const {return mBuff;}
	void	setDataLen(const u32 len) {mDataLen = len;}
	u32		getDataLen() const {return mDataLen;}
	bool	setBuff(const u32 size)
			{
				if (mMemSize >= size) return true;

				if (mBuff) OmnDelete [] mBuff;
				mBuff = OmnNew char[size];
				aos_assert_r(mBuff, false);
				mMemSize = size;
				return true;
			}
};

#endif
