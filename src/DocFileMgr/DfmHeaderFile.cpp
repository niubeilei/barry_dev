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

#include "DocFileMgr/DfmHeaderFile.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h" 
#include "DocFileMgr/BodyFile.h"
#include "ReliableFile/ReliableFile.h"
#include "Util/Buff.h" 

AosDfmHeaderFile::AosDfmHeaderFile(const bool regflag)
:
AosDfmFile(AosDfmFileType::eHeaderFile, regflag)
{
}


AosDfmHeaderFile::AosDfmHeaderFile(
		const AosRundataPtr &rdata,
		const u32 seqno,
		const u32 snap_id,
		const u32 vid,
		const OmnString &prefix,
		const u64 max_fsize)
:
AosDfmFile(rdata, AosDfmFileType::eHeaderFile, seqno, vid, prefix, max_fsize),
mSnapId(snap_id)
{
}

AosDfmHeaderFile::~AosDfmHeaderFile()
{
}


AosDfmFilePtr
AosDfmHeaderFile::clone()
{
	return OmnNew AosDfmHeaderFile(false);
}

	
bool
AosDfmHeaderFile::serializeFrom(const AosBuffPtr &buff)
{
	bool rslt = AosDfmFile::serializeFrom(buff); 
	aos_assert_r(buff && rslt, 0);
	
	mSnapId = buff->getU32(0);
	return true;
}


bool
AosDfmHeaderFile::serializeTo(const AosBuffPtr &buff)
{
	bool rslt = AosDfmFile::serializeTo(buff); 
	aos_assert_r(buff && rslt, 0);
	
	buff->setU32(mSnapId);
	return true;
}


