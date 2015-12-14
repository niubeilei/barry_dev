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
// 11/05/2012	Created by Ketty
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_ReliableFile_RlbFileNotSend_h
#define AOS_ReliableFile_RlbFileNotSend_h

#include "ReliableFile/ReliableFile.h"

class AosRlbFileNotSend: public AosReliableFile
{
	OmnDefineRCObject;

public:
	AosRlbFileNotSend();
	AosRlbFileNotSend(const AosStorageFileInfo &file_info, const bool create_flag, AosRundata *);
	~AosRlbFileNotSend();

	virtual AosReliableFilePtr clone(const AosStorageFileInfo &file_info, 
									const bool create_flag, 
									AosRundata *rdata);

	virtual bool	modifyFile(const u64 offset,
				const char *data,
				const int len,
				const bool flushflag,
				bool &file_damaged,		// Ketty 2012/10/08
				AosRundata *rdata);
	

};
#endif
