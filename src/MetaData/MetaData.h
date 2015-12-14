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
// 2015/03/24 Created by Jackie 
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_MetaData_METADATA_H
#define AOS_MetaData_METADATA_H

#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/Opr.h"
#include "MetaData/Ptrs.h"

#include <map>

using namespace std;

class AosMetaData : public OmnRCObject
{
	OmnDefineRCObject;

public:
	OmnString							mType;
	int64_t								mRecordLength;

public:
	AosMetaData();
	~AosMetaData();

	AosMetaData & operator = (const AosMetaData &rhs)
	{
		mType = rhs.mType;
		return *this;
	}

	OmnString getType() { return mType; }

	virtual void setRecordLength(int64_t &length);
	virtual void moveToNext() = 0;
	virtual void moveToCrt() = 0;
	virtual void moveToBack(int64_t len) = 0;

	virtual OmnString getAttribute(const OmnString &key) = 0;

};

#endif
