////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
//
// Modification History:
// 2015/01/27 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "Util/WordIdMgr.h"
#include "Util/ValueRslt.h"

#include "Rundata/Rundata.h"


AosWordIdMgr::AosWordIdMgr(AosRundata *rdata, const OmnString &objid)
:
mObjid(objid),
mDocid(0)
{
	if (!loadMapByObjid(rdata))
	{
		OmnThrowException(rdata->getErrmsg());
		return;
	}
}


AosWordIdMgr::AosWordIdMgr(AosRundata *rdata, const u64 docid)
:
mDocid(docid)
{
	if (!loadMapByDocid(rdata))
	{
		OmnThrowException(rdata->getErrmsg());
		return;
	}
}


AosWordIdMgr::~AosWordIdMgr()
{
}


bool
AosWordIdMgr::loadMapByObjid(AosRundata *rdata)
{
	// The map is stored in the doc identified by mObjid.
	// This function retrieves the doc and builts the
	// map from it.
	OmnNotImplementedYet;
	return false;
}


bool
AosWordIdMgr::loadMapByDocid(AosRundata *rdata)
{
	// The map is stored in the doc identified by mDocid.
	// This function retrieves the doc and builts the
	// map from it.
	OmnNotImplementedYet;
	return false;
}


u32 
AosWordIdMgr::getWordId(
		AosRundata *rdata, 
		const OmnString &word, 
		const bool create_flag)
{
	// This function checks whether 'word' is in the map.
	// If yes, it returns the word id. Otherwise, if 'create_flag'
	// is true, it creates the entry. Otherwise, it returns 0
	// (which is an invalid word ID).
	//
	// This function must be thread safe.

	OmnNotImplementedYet;
	return 0;
}

