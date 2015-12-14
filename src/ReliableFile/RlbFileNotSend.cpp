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
// 11/05/2012 Created by Ketty 
////////////////////////////////////////////////////////////////////////////
#include "ReliableFile/RlbFileNotSend.h"

#include "Rundata/Rundata.h"


AosRlbFileNotSend::AosRlbFileNotSend()
:
AosReliableFile(AosRlbFileType::eNotSend)
{
}


AosRlbFileNotSend::AosRlbFileNotSend(
		const AosStorageFileInfo &file_info,
		const bool create_flag,
		AosRundata *rdata)
:
AosReliableFile(AosRlbFileType::eNotSend, file_info, create_flag, rdata)
{
}


AosRlbFileNotSend::~AosRlbFileNotSend()
{
}


AosReliableFilePtr
AosRlbFileNotSend::clone(
		const AosStorageFileInfo &file_info, 
		const bool create_flag, 
		AosRundata *rdata)
{
	try
	{
		return OmnNew AosRlbFileNotSend(file_info, create_flag, rdata);
	}

	catch (...)
	{
		AosSetErrorU(rdata, "failed_create_obj") << ": " << file_info.fileId << enderr;
		return 0;
	}
}


bool
AosRlbFileNotSend::modifyFile(
		const u64 offset, 
		const char *data, 
		const int len, 
		const bool flush, 
		bool &file_damaged,		// Ketty 2012/10/08
		AosRundata *rdata)
{
	bool rslt = modifyLocalFile(offset, data, len, flush, rdata);
	aos_assert_r(rslt, false);
	return true;
}


