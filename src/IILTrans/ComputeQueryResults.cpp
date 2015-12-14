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
// 2013/03/01 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#if 0
#include "IILTrans/ComputeQueryResults.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Debug/Except.h"
#include "TransUtil/XmlTrans.h"
#include "SEInterfaces/QueryContextObj.h"
#include "Util/Buff.h"
#include "XmlUtil/XmlTag.h"


AosIILTransComputeQueryResults::AosIILTransComputeQueryResults(const bool flag)
:
AosIILTrans(AosIILFuncType::eComputeQueryResults, flag AosMemoryCheckerArgs)
{
}


bool
AosIILTransComputeQueryResults::initTrans(
		const AosXmlTransPtr &trans,
		const AosXmlTagPtr &trans_doc,
		const AosRundataPtr &rdata)
{
	aos_assert_rr(trans_doc, rdata, false);
	mTransId = trans->getTransId();
	bool rslt = AosIILTrans::initTransBaseById(trans_doc, rdata);
	if(!rslt) return rslt;

	AosXmlTagPtr qrycontext = trans_doc->getFirstChild("query_context");
	aos_assert_rr(qrycontext, rdata, false);
	mQueryContext = AosQueryContextObj::createQueryContextStatic();
	mQueryContext->serializeFromXml(qrycontext);
	mQueryId = trans_doc->getAttrU64(AOSTAG_QUERY_ID, 0);
	mPhysicalId = trans_doc->getAttrInt(AOSTAG_Physical_ID, -1);
	aos_assert_rr(mPhysicalId >= 0, rdata, false);

	mBitmap = trans_doc->getNodeTextBinaryCopy(AOSTAG_BITMAP AosMemoryCheckerArgs);
	aos_assert_rr(mBitmap, rdata, false);
	return true;
}


AosIILTransPtr 
AosIILTransComputeQueryResults::clone()
{
	return OmnNew AosIILTransComputeQueryResults(false);
}


bool
AosIILTransComputeQueryResults::serializeFrom(const AosBuffPtr &buff)
{
	OmnNotImplementedYet;
	return false;
}


bool
AosIILTransComputeQueryResults::serializeTo(const AosBuffPtr &buff)
{
	OmnNotImplementedYet;
	return false;
}


bool
AosIILTransComputeQueryResults::proc(const AosIILObjPtr &iilobj, const AosRundataPtr &rdata)
{
	aos_assert_rr(iilobj, rdata, false);

	AosBuffPtr buff;
	bool rslt = iilobj->computeQueryResults(rdata, 
			mQueryContext, mBitmap, mQueryId, mPhysicalId); 
	aos_assert_rr(rslt, rdata, false);
	return true;
}


u64 
AosIILTransComputeQueryResults::getIILID(const AosRundataPtr &rdata)
{
	return getIILIDById(rdata);
}


AosIILType 
AosIILTransComputeQueryResults::getIILType() const
{
	return eAosIILType_Str;
}

	
int
AosIILTransComputeQueryResults::getSerializeSize() const
{
	OmnNotImplementedYet;
	return 0;
}
#endif
