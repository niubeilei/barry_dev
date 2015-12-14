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
// 01/02/2013 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "QueryProcDyn/BitmapQueryProc.h"

#include "Jimo/JimoUtil.h"
#include "QueryEngine/BitmapQueryTermOr.h"
#include "SEInterfaces/QueryReqObj.h"
#include "SEInterfaces/QueryProcCallback.h"


static AosJimoUtil::funcmap_t sgFuncMap;
static bool sgInited = false;
static OmnMutex sgLock;

extern "C"
{

AosJimoPtr AosCreateJimoFunc_AosBitmapQueryProc_0(
 		const AosRundataPtr &rdata, 
 		const AosXmlTagPtr &worker_doc, 
 		const AosXmlTagPtr &jimo_doc, 
		const u32 version) 
{
	try
	{
		OmnScreen << "To create Jimo: " << endl;
		return OmnNew AosBitmapQueryProc(version);
	}

	catch (...)
	{
		AosSetErrorU(rdata, "Failed creating jimo") << enderr;
		return 0;
	}

	OmnShouldNeverComeHere;
	return 0;
}
}


AosBitmapQueryProc::AosBitmapQueryProc(const u32 version)
:
AosQueryProc(version),
mVersion(version)
{
}


/*AosBitmapQueryProc::AosBitmapQueryProc(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &worker_doc, 
		const AosXmlTagPtr &jimo_doc, 
		const OmnString &version)
:
AosQueryProc(rdata, worker_doc, jimo_doc),
mVersion(version),
mJimoName("bitmap_query_proc")
{
	if (!init(rdata))
	{
		OmnThrowException(rdata->getErrmsg());
	}
}
*/

bool
AosBitmapQueryProc::init(const AosRundataPtr &rdata)
{
	if (!sgInited) registerMethods();
	return true;
}


bool
AosBitmapQueryProc::registerMethods()
{
	sgLock.lock();
	if (sgInited)
	{
		sgLock.unlock();
		return true;
	}

	// registerMethod("next_value", sgFuncMap, 
	// 		AosMethodId::eAosRundata_Jimo_WorkerDoc,
	// 		(void *)AosBitmapQueryProc_nextValue);

	sgInited = true;
	sgLock.unlock();
	return true;
}


AosBitmapQueryProc::~AosBitmapQueryProc()
{
	mQueryReq = 0;
	if(mOrTerm)
	{
		mOrTerm->clear();
	}
	mOrTerm = 0;
}


bool	
AosBitmapQueryProc::runQuery(
		const AosRundataPtr &rdata, 
		const AosQueryReqObjPtr &query_req, 
		const AosQueryProcCallbackPtr &callback)
{
	// In the current implementations, it is a synched call. 
	// It should be converted to an asynched call in the future.
	aos_assert_rr(callback, rdata, false);
	if (!mCallback)
	{
		// It is a new query
		mCallback = callback;

		mQueryReq = query_req;
		aos_assert_r(mQueryReq, false);
		AosQueryTermObjPtr term = mQueryReq->getQueryTerm();
		aos_assert_r(term, false);
		AosQueryProcObjPtr thisptr(this, false);
		mOrTerm = AosBitmapQueryTermOr::createStatic(rdata, thisptr, term, query_req);
		aos_assert_r(mOrTerm, false);
	}

	// Note that this is a synced call for now.
	bool rslt = mOrTerm->runQuery(rdata);
	if (!rslt)
	{
		mCallback->queryFailed(rdata, rdata->getErrmsg());
		return false;
	}
	return true;	
	
	// mQueryReq->setQueryContext(mOrTerm->getQueryContext());
	// mQueryReq->setQueryData(mOrTerm->getQueryRslt());
	return true;
}


bool
AosBitmapQueryProc::config(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &worker_doc)
{
	return true;
}


OmnString
AosBitmapQueryProc::toString() const
{
	return "";
}


bool 
AosBitmapQueryProc::serializeTo(
		const AosRundataPtr &rdata, 
		const AosBuffPtr &buff) 
{
	// bool rslt = AosQueryProc::serializeTo(rdata, buff);
	bool rslt = AosJimo::serializeTo(rdata, buff);
	aos_assert_rr(rslt, rdata, false);
	return true;
}


bool 
AosBitmapQueryProc::serializeFrom(
		const AosRundataPtr &rdata, 
		const AosBuffPtr &buff)
{
	// bool rslt = AosQueryProc::serializeFrom(rdata, buff);
	bool rslt = AosJimo::serializeFrom(rdata, buff);
	aos_assert_rr(rslt, rdata, false);
	return true;
}


AosJimoPtr 
AosBitmapQueryProc::cloneJimo(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &worker_doc,
		const AosXmlTagPtr &jimo_doc)  const
{
	try
	{
		return OmnNew AosBitmapQueryProc(*this);
	}

	catch (...)
	{
		OmnAlarm << "Failed creating jimo" << enderr;
		return 0;
	}
}


bool 
AosBitmapQueryProc::run(const AosRundataPtr &rdata) 
{
	OmnNotImplementedYet;
	return false;
}


bool 
AosBitmapQueryProc::run(const AosRundataPtr &rdata, const AosXmlTagPtr &doc) 
{
	return AosJimo::run(rdata, doc);
}


void * 
AosBitmapQueryProc::getMethod(
		const AosRundataPtr &rdata, 
		const OmnString &name, 
		AosMethodId::E &method_id)
{
	return AosJimo::getMethod(rdata, name, sgFuncMap, method_id);
}


bool 
AosBitmapQueryProc::queryInterface(
		const AosRundataPtr &rdata, 
		const OmnString &interface_objid) const
{
	return AosJimo::queryInterface(rdata, interface_objid);
}

int
AosBitmapQueryProc::getPageSize() const
{
	aos_assert_r(mQueryReq, 0);
	return mQueryReq->getPageSize();
}

bool 
AosBitmapQueryProc::queryFinished(
		const AosRundataPtr &rdata,
		const AosQueryRsltObjPtr &data)
{
	aos_assert_rr(mQueryReq, rdata, false);
	aos_assert_rr(mCallback, rdata, false);
	OmnString records;
	bool rslt = mQueryReq->generateResults(rdata, data, records);
	if (!rslt)
	{
		return mCallback->queryFailed(rdata, rdata->getErrmsg());
	}
	return mCallback->queryFinished(rdata, records);
}


bool 
AosBitmapQueryProc::queryFailed(
		const AosRundataPtr &rdata,
		const OmnString &errmsg)
{
	aos_assert_rr(mQueryReq, rdata, false);
	aos_assert_rr(mCallback, rdata, false);
	mCallback->queryFailed(rdata, errmsg);
	return true;
}


u64
AosBitmapQueryProc::getQueryId() const
{
	aos_assert_r(mQueryReq, 0);
	return mQueryReq->getQueryId();
}


bool 
AosBitmapQueryProc::queryProcResponded(
		const AosRundataPtr &rdata, 
		const OmnString &errmsg, 
		const AosBitmapObjPtr &bitmap)
{
	aos_assert_rr(mOrTerm, rdata, false);
	return mOrTerm->queryProcResponded(rdata, errmsg, bitmap);
}

AosJimoPtr 
AosBitmapQueryProc::cloneJimo()  const
{
	try
	{
		return OmnNew AosBitmapQueryProc(*this);
	}

	catch (...)
	{
		OmnAlarm << "Failed creating jimo" << enderr;
		return 0;
	}
}



