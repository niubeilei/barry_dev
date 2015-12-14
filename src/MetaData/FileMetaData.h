////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appoint64_ted officer of Packet Engineering, Inc. or its derivatives
//	
// Description:
//
// Modification History:
// 2015/03/24 Created by Jackie 
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_MetaData_FILEMETADATA_H
#define AOS_MetaData_FILEMETADATA_H

#include "MetaData/MetaData.h"
#include "MetaData/Ptrs.h"
#include "Util/String.h"
#include "Rundata/Ptrs.h"

class AosFileMetaData : public AosMetaData
{
	OmnString							mFileName;
	int64_t 							mFileOffset; // block offset of the file
	int64_t								mRecordOffset; 
	
private:

public:
	AosFileMetaData(OmnString &filename, int64_t offset);
	~AosFileMetaData();

	virtual void moveToNext();
	virtual void moveToCrt();
	virtual void moveToBack(int64_t len);
	virtual OmnString getAttribute(const OmnString &key);
};

#endif
