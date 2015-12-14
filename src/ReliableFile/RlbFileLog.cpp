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
// 09/24/2012 Created by Ketty 
////////////////////////////////////////////////////////////////////////////
#include "ReliableFile/RlbFileLog.h"

#include "Rundata/Rundata.h"


AosRlbFileLog::AosRlbFileLog()
:
AosReliableFile(AosRlbFileType::eLog)
{
}


AosRlbFileLog::AosRlbFileLog(
		const AosStorageFileInfo &file_info,
		const bool create_flag,
		AosRundata *rdata)
:
AosReliableFile(AosRlbFileType::eLog, file_info, create_flag, rdata)
{
	if(!mIsNewFile)	return;
	
	//AosBuffPtr fmt_buff = initFmtBuff(mFileInfo);
	//aos_assert_r(fmt_buff, false);
	
	//AosReplicMgr::getSelf()->addFmt(rdata, 
	//		OmnGetCurrentThreadId(), fmt_buff);
}


AosRlbFileLog::~AosRlbFileLog()
{
}


AosReliableFilePtr
AosRlbFileLog::clone(
		const AosStorageFileInfo &file_info, 
		const bool create_flag, 
		AosRundata *rdata)
{
	try
	{
		return OmnNew AosRlbFileLog(file_info, create_flag, rdata);
	}

	catch (...)
	{
		AosSetErrorU(rdata, "failed_create_obj") << ": " << file_info.fileId << enderr;
		return 0;
	}
}


bool
AosRlbFileLog::modifyFile(
		const u64 offset, 
		const char *data, 
		const int len, 
		const bool flush, 
		bool &file_damaged,		// Ketty 2012/10/08
		AosRundata *rdata)
{
	aos_assert_rr(modifyLocalFile(offset, data, len, flush, rdata), rdata, false);
	
	//AosBuffPtr fmt_buff = initFmtBuff(offset, data, len, flushflag);
	//aos_assert_r(fmt_buff, false);

	//return AosReplicMgr::getSelf()->addFmt(rdata,
	//		OmnGetCurrentThreadId(), fmt_buff);
	return true;
}


