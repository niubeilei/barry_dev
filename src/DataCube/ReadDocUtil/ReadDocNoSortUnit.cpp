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
// 2014/01/21 Created by Ketty 
////////////////////////////////////////////////////////////////////////////
#include "DataCube/ReadDocUtil/ReadDocNoSortUnit.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "API/AosApi.h"
#include "API/AosApiG.h"
#include "Rundata/Rundata.h"
#include "XmlUtil/Ptrs.h"
#include "XmlUtil/XmlTag.h"


AosReadDocNoSortUnit::AosReadDocNoSortUnit(
		const AosDocOprPtr &doc_opr,
		const u32 dist_id,
		IdVector &docids)
:
mDocOpr(doc_opr),
mDistId(dist_id),
mDocids(docids),
mReadedNum(0)
{
}

AosReadDocNoSortUnit::AosReadDocNoSortUnit()
{
}

AosReadDocNoSortUnit::~AosReadDocNoSortUnit()
{
}

void
AosReadDocNoSortUnit::include()
{
	OmnNew AosReadDocNoSortUnit();
}


bool
AosReadDocNoSortUnit::config(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &worker_doc)
{
	OmnShouldNeverComeHere;
	return true;
}


AosDataConnectorObjPtr 
AosReadDocNoSortUnit::cloneDataConnector()
{
	OmnShouldNeverComeHere;
	return 0;
}


AosJimoPtr 
AosReadDocNoSortUnit::cloneJimo() const
{
	OmnShouldNeverComeHere;
	return 0;
}

void
AosReadDocNoSortUnit::setCaller(const AosDataConnectorCallerObjPtr &caller)
{
	mCaller = caller;
}


bool
AosReadDocNoSortUnit::readData(const u64 reqid, const AosRundataPtr &rdata)
{
	u64 total_num = mDocids.size();
	if(isReadFinish())
	{
		mCaller->callBack(reqid, 0, true);
		return true;
	}

	u64 need_read_num = total_num - mReadedNum;
	if(need_read_num > eEachNumPerRead)	need_read_num = eEachNumPerRead;

	IdVector docids;
	for(u32 i=0, idx = mReadedNum; i< need_read_num; i++, idx++)
	{
		aos_assert_r(idx < total_num, false);	
		docids.push_back(mDocids[idx]);
	}

	mReadedNum += need_read_num;

	AosAsyncRespCallerPtr resp_caller(this, false);
	return mDocOpr->sendReadDocTrans(rdata, reqid, mDistId, docids, resp_caller);
}


bool
AosReadDocNoSortUnit::isReadFinish()
{
	return mReadedNum == mDocids.size();
}


void
AosReadDocNoSortUnit::callback(
		const AosTransPtr &trans, 
		const AosBuffPtr &resp, 
		const bool svr_death)
{
	// this is trans's resp callback.
	OmnNotImplementedYet;
	
	aos_assert(svr_death && resp);
	
	//u64 data_scan_reqid = resp->getU64(0);	
	//mCaller->callBack(data_scan_reqid, resp, isReadFinish());
}


