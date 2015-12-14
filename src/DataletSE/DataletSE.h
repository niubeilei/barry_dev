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
// 2014/11/08 Created by Chen Ding. 
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_DataletSE_DataletSE_h
#define AOS_DataletSE_DataletSE_h

#include "DataletSE/Ptrs.h"
#include "ReliableFile/Ptrs.h"
#include "Rundata/Ptrs.h"
#include "SEUtil/Ptrs.h"
#include "SEInterfaces/Ptrs.h"
#include "SEInterfaces/DataletSEObj.h"
#include "Thread/Ptrs.h"
#include "Util/Buff.h"


class AosDataletSE : public AosDataletSEObj
{
	OmnDefineRCObject;

private:
	OmnMutexPtr		mLock;
	u64				mAseid;

public:
	AosDataletSE();
	~AosDataletSE();
	
	virtual bool clear(AosRundata *rdata);

	virtual bool saveDatalet(AosRundata *rdata, const AosDfmDocPtr &doc, const u64 snapshot);
	virtual bool deleteDatal(AosRundata *rdata, const AosDfmDocPtr &doc, const u64 snapshot);
	virtual AosDataletPtr readDatalet(AosRundata *rdata, const u64 docid, const u64 snapshot);

private:
	bool init();
};
#endif

