////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appoint64_ted officer of Packet Engineering, Inc. or its derivatives
//	
// Description:
//
// Modification History:
// 2015/03/24 Created by Jackie 
////////////////////////////////////////////////////////////////////////////
#include "MetaData/FileMetaData.h"

AosFileMetaData::AosFileMetaData(OmnString &filename, int64_t offset)
:
mFileName(filename),
mFileOffset(offset)
{
	mRecordOffset = mFileOffset;
}


AosFileMetaData::~AosFileMetaData()
{
}


OmnString 
AosFileMetaData::getAttribute(const OmnString &key)
{
	OmnString ss = "";
	if (key == "filename") return mFileName;
	if (key == "offset")
	{
		ss << mRecordOffset;
		return ss;
	}
	if (key == "length") 
	{
		ss << mRecordLength;
		return ss;
	}
	return ss;
}


void 
AosFileMetaData::moveToNext()
{
	mRecordOffset += mRecordLength;
}


void 
AosFileMetaData::moveToBack(int64_t len)
{
	mFileOffset -= len;
	mRecordOffset = mFileOffset;
}


void 
AosFileMetaData::moveToCrt()
{
	mFileOffset = mRecordOffset;
}
