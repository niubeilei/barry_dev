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
// 08/14/2012	Created by Ketty
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_StorageMgr_RlbFileNormal_h
#define AOS_StorageMgr_RlbFileNormal_h

#include "ReliableFile/ReliableFile.h"
#include "Util/File.h"
#include "Util/RCObjImp.h"
#include "Util/Ptrs.h"

class AosRlbFileNormal: public AosReliableFile
{
	OmnDefineRCObject;
	
public:
	AosRlbFileNormal();
	AosRlbFileNormal(const AosStorageFileInfo &file_info, const bool create_flag, AosRundata *);
	~AosRlbFileNormal();

	virtual AosReliableFilePtr clone(const AosStorageFileInfo &file_info, 
									const bool create_flag, 
									AosRundata *rdata);

	virtual bool	modifyFile(const u64 offset,
				const char *data,
				const int len,
				const bool flushflag,
				bool &file_damaged,
				AosRundata *rdata);
};
#endif
