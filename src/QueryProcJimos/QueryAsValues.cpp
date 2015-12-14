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
// Here is an example:
//
// select t.order_no,
//   (
//      select se.s_name
//      from se
//      where se.order_no = t.order_no
//   ) order_name,
//   t.unit_id
// from t;
//
// In this case, the subquery is used as a selected field. It will be 
// implemented through a map and a virtual field. The input to the
// virtual field is 't.order_no', and the outputs are values for
// 'se.s_name'. The map is defined on 'se', and the virtual field
// is defined on 't'. 
//
// If the virtual field is present, the original select is replaced
// as:
// 		select t.order_no, t._vf_se_s_name, t.unit_id from t;
//
// Modification History:
// 2015/01/31 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "Query/QueryAsValues.h"



AosQueryAsValues::AosQueryAsValues(
		AosRundata *rdata, 
		const AosJqlSelectObjPtr &orig_statement, 
		const AosJqlSelectObjPtr &crt_statement)
:
mOrigStatement(orig_statement),
mCrtStatement(crt_statement)
{
}


AosQueryAsValues::~AosQueryAsValues()
{
}


bool
AosQueryAsValues::config()
{
	return true;
}


bool
AosQueryAsValues::runQuery(AosRundata *rdata)
{
	// 1. Check whether the map exist.
	OmnString map_name = createMapName();
	if (!mSubtable->joinMapExist(rdata, map_name))
	{
		return buildJoinMap(rdata);
	}

	if (!mMasterTable->virtualFieldExist(rdata, xxx))
	{
		return buildVirtualField(rdata);
	}

	return modifyOriginalStatement(rdata);
}


bool
AosQueryAsValues::analyzeQuery(const AosRundataPtr &rdata)
{
}


OmnString 
AosQueryAsValues::toString() const
{
	return "QueryAsValues: not implemented yet!";
}


bool 
AosQueryAsValues::queryFinished(
		const AosRundataPtr &rdata,
		const AosQueryRsltObjPtr &results,
		const AosQueryProcObjPtr &proc)
{
	mLock1->lock();
	mOrigQueryRslt = results;
	mTotal = proc->getTotalInRslt();
	mCondVar->signal();
	mLock1->unlock();
	return true;
}


bool 
AosQueryAsValues::queryFailed(
		const AosRundataPtr &rdata,
		const OmnString &errmsg)
{
	mLock1->lock();
	mOrigQueryRslt = 0;
	mCondVar->signal();
	mLock1->unlock();
	return true;
}

