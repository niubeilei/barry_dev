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
// 04/08/2013	Created by Ketty
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_DocFileMgr_DfmSlabFile_h
#define AOS_DocFileMgr_DfmSlabFile_h

#include "DocFileMgr/DfmFile.h"

class AosDfmSlabFile: public AosDfmFile
{
private:
	u32			mBlockSize;
	AosSlabBlockFilePtr	mSlabFile;

public:
	AosDfmSlabFile(const bool regflag);
	AosDfmSlabFile(
		const AosRundataPtr &rdata,
		const u32 blocksize,
		const u32 seqno,
		const u32 vid,
		const OmnString &prefix,
		const u64 max_fsize);
	~AosDfmSlabFile();

	virtual AosDfmFilePtr clone();

	AosSlabBlockFilePtr getSlabFile(const AosRundataPtr &rdata);

};

#endif
