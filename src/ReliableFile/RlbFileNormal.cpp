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
// 08/14/2012 Created by Ketty 
////////////////////////////////////////////////////////////////////////////
#include "ReliableFile/RlbFileNormal.h"

#include "API/AosApiG.h"
#include "Util/Buff.h"
#include "Rundata/Rundata.h"

AosRlbFileNormal::AosRlbFileNormal()
:
AosReliableFile(AosRlbFileType::eNormal)
{
}


AosRlbFileNormal::AosRlbFileNormal(
		const AosStorageFileInfo &file_info, 
		const bool create_flag, 
		AosRundata *rdata)
:
AosReliableFile(AosRlbFileType::eNormal, file_info, create_flag, rdata)
{
	if(!mIsNewFile)	return;

	/*
	AosBuffPtr fmt_buff = initFmtBuff(mFileInfo);
	aos_assert_r(fmt_buff, false);
	
	u32 virtual_id = (u32)(mFileId >> 32);
	bool rslt = AosReplicMgr::getSelf()->sendFmt(rdata, virtual_id, fmt_buff);
	if(!rslt)
	{
		OmnAlarm << "send Fmt error!" << enderr;
		OmnThrowException(rdata->getErrmsg());
	}
	*/
}


AosRlbFileNormal::~AosRlbFileNormal()
{
}


AosReliableFilePtr
AosRlbFileNormal::clone(
		const AosStorageFileInfo &file_info, 
		const bool create_flag, 
		AosRundata *rdata)
{
	try
	{
		return OmnNew AosRlbFileNormal(file_info, create_flag, rdata);
	}
	catch (...)
	{
		AosSetErrorU(rdata, "failed_creating_reliable_file") << file_info.toString() << enderr;
		return 0;
	}
}


bool
AosRlbFileNormal::modifyFile(
		const u64 offset,
		const char *data,
		const int len,
		const bool flushflag,
		bool &file_damaged,		// Ketty 2012/10/08
		AosRundata *rdata)
{
	aos_assert_rr(modifyLocalFile(offset, data, len, flushflag, rdata), rdata, false);

	/*
	AosBuffPtr fmt_buff = initFmtBuff(offset, data, len, flushflag);
	aos_assert_r(fmt_buff, false);

	u32 virtual_id = (u32)(mFileId >> 32);
	bool rslt = AosReplicMgr::getSelf()->sendFmt(rdata, virtual_id, fmt_buff);
	aos_assert_r(rslt, false);
	*/
	return true;
}


