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
#ifndef AOS_DocFileMgr_DfmDiskFile_h
#define AOS_DocFileMgr_DfmDiskFile_h

#include "DocFileMgr/DfmFile.h"

class AosDfmDiskFile: public AosDfmFile
{
private:
	u32			mBlockSize;
	AosBodyFilePtr mDiskFile;

public:
	AosDfmDiskFile(const bool regflag);
	AosDfmDiskFile(
		const AosRundataPtr &rdata,
		const u32 seqno,
		const u32 block_size,
		const u32 vid,
		const OmnString &prefix,
		const u64 max_fsize);
	~AosDfmDiskFile();

	virtual bool serializeFrom(const AosBuffPtr &buff);
	virtual bool serializeTo(const AosBuffPtr &buff);
	virtual AosDfmFilePtr clone();

	AosBodyFilePtr getDiskFile(const AosRundataPtr &rdata);
	bool 	closeFile(); 		// Ketty 2014/02/21

};

#endif
