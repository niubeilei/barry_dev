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
// 2013/10/29 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SuperTable_SuperTableNorm_h
#define Aos_SuperTable_SuperTableNorm_h

#include "SuperTable/SuperTable.h"


class AosSuperTableNorm : public AosSuperTable
{
	OmnDefineRCObject;

private:
	AosDatasetObjPtr	mDataset;

public:
	AosSuperTableNorm(const AosRundataPtr &rdata, 
			const AosXmlTagPtr &doc);
	~AosSuperTableNorm();

	virtual bool reset(const AosRundataPtr &rdata);
	virtual AosDataRecordObjPtr nextRecord(const AosRundataPtr &rdata);
	virtual int64_t getNumEntries(const AosRundataPtr &rdata);

private:
	bool init(const AosRundataPtr &rdata);
	bool createDataset(const AosRundataPtr &rdata);
};
#endif

