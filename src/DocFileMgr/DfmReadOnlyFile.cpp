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

#include "DocFileMgr/DfmReadOnlyFile.h"
#include "Util/OmnNew.h"


AosDfmReadOnlyFile::AosDfmReadOnlyFile(const bool regflag)
:
AosDfmFile(AosDfmFileType::eReadOnlyFile, regflag)
{
}

AosDfmReadOnlyFile::AosDfmReadOnlyFile(
		const AosRundataPtr &rdata,
		const u32 seqno,
		const u32 vid,
		const OmnString &prefix,
		const u64 max_fsize)
:
AosDfmFile(rdata, AosDfmFileType::eReadOnlyFile, seqno, vid, prefix, max_fsize)
{
}

AosDfmReadOnlyFile::~AosDfmReadOnlyFile()
{
}

	
AosDfmFilePtr
AosDfmReadOnlyFile::clone()
{
	return OmnNew AosDfmReadOnlyFile(false);
}


