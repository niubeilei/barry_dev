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
#ifndef AOS_MetaData_DOCIDMETADATA_H
#define AOS_MetaData_DOCIDMETADATA_H

#include "MetaData/MetaData.h"
#include "MetaData/Ptrs.h"

#include <vector>

class AosDocidMetaData : public AosMetaData
{	
private:
	vector<u64>	mDocids;
	vector<u64>	mSchemaIds;
	u64			mIdx;

public:
	AosDocidMetaData(){}
	AosDocidMetaData(vector<u64> &docids, vector<u64> &schemaids);
	~AosDocidMetaData();

	virtual void moveToNext();
	virtual void moveToCrt() {}
	virtual void moveToBack(int64_t len) {}
	virtual OmnString getAttribute(const OmnString &key);
};

#endif
