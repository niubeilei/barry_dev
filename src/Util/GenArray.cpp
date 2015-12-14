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
// AosGenArray contains an array of OmnStrings, integers, and/or
// AosGenTables. 
// Each of them serves as a data field. Each data field is accessed
// through indices. When creating the instance, one tells how many
// each type of fields is. Each instance of this class serves as a record. 
//
// Modification History:
// 2014/01/03: Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#if 0
#include "Util/GenArray.h"


AosBuffPtr		mBuff;
	AosDataType::E	mDataType;
	int				mNumElems;

AosGenArray::AosGenArray()
{
}


AosGenArray::AosGenArray(
		const AosDataType::E type, 
		const char endian,
		const char *data, 
		const int len, 
		const bool need_copy)
:
mDataType(type),
mEndian(endian)
{
	if (need_copy)
	{
		mBuff = OmnNew AosBuff(data, len, true);
	}
	else
	{
		mBuff = OmnNew AosBuff();
		mBuff->setData(data, len, false);
	}
	mBuff->setDataLen(len);
	mData = mBuff->data();
}


AosGenArray::~AosGenArray()
{
}


bool 
AosGenArray::getElem(const int idx, AosValueRslt &value)
{
	aos_assert_r(idx >= 0, false);
	aos_assert_r(idx < mNumElems, false);
	int nn;
	switch (mDataType)
	{
	case AosDataType::eU8:
		 aos_assert_r(idx < mDataLen, false);
		 value.setU8((u8)mData[idx]);
		 return true;

	case AosDataType::eU32:
		 nn = idx*sizeof(u32);
		 aos_assert_r(nn + sizeof(u32) < mDataLen, false);
		 return value.setU32(*(u32*)&mData[nn], mEndian);

	case AosDataType::eBinary:
		 nn = idx*mElemSize;
		 aos_assert_r(nn + mElemSize < mDataLen, false);
		 return value.setBinary(&mData[nn], mElemSize);

	case AosDataType::eEmbeddedDoc:
		 if (!mIndex) createIndex();
		 aos_assert_r(mIndex, false);
		 return value.setEmbeddedDoc(&mData[mIndex[idx]], mLength[idx]);
	}
}
#endif
