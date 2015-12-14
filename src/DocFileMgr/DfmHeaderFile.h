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
#ifndef AOS_DocFileMgr_DfmHeaderFile_h
#define AOS_DocFileMgr_DfmHeaderFile_h

#include "DocFileMgr/DfmFile.h"

class AosDfmHeaderFile: public AosDfmFile
{

private:
	u32		mSnapId;

public:
	AosDfmHeaderFile(const bool regflag);
	AosDfmHeaderFile(
		const AosRundataPtr &rdata,
		const u32 seqno,
		const u32 snap_id,
		const u32 vid,
		const OmnString &prefix,
		const u64 max_fsize);
	~AosDfmHeaderFile();

	virtual bool serializeFrom(const AosBuffPtr &buff);
	virtual bool serializeTo(const AosBuffPtr &buff);
	virtual AosDfmFilePtr clone();
	
	u32	getSnapId(){ return mSnapId; };

};

#endif
