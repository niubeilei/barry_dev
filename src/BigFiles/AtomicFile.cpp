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
//
// Modification History:
// 2013/04/20 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "BigFile/RawFile.h"


AosRawFile::AosRawFile(
		const int phyid, 
		const OmnString &fname)
:
AosAtomicFile(phyid),
mFname(fname)
{
}


AosRawFile::~AosRawFile()
{
}


int64_t 
AosRawFile::getFileLength() const
{
	aos_assert_r(mFname != "", -1);
	if (isLocal())
	{
		return OmnGetFileLength(fname.data());
	}
	return -1;
}

