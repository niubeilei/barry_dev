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
// This condition is used to search for tags. There is an IIL for each
// tag:
// 	TagIIL: <docid> <docid> ... <docid>
// The IIL is sorted based on docids. 
//
// Modification History:
// 08/07/2011	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "Query/TermHitType.h"

#include "Alarm/Alarm.h"
#include "Query/QueryReq.h"
#include "SEUtil/IILName.h"
#include "Rundata/Rundata.h"
#include "SEInterfaces/BitmapObj.h"
#include "WordParser/WordNorm.h"
#include "XmlUtil/XmlTag.h"


AosTermHitType::AosTermHitType(const AosQueryType::E type)
:
AosTermIILType(AOSTERMTYPE_HITTYPE, type, false)
{
}


AosTermHitType::AosTermHitType(
		const OmnString &name, 
		const AosQueryType::E type, 
		const bool flag)
:
AosTermIILType(name, type, flag)
{
}


bool 
AosTermHitType::getDocidsFromIIL(
		const AosQueryRsltObjPtr &query_rslt, 
		const AosBitmapObjPtr &query_bitmap, 
		const AosQueryContextObjPtr &query_context,
		const AosRundataPtr &rdata)
{
	// if (mIILId)
	// {
	// 	return AosIILClient::getSelf()->querySafe(mIILId, query_rslt, query_bitmap, rdata);
	// }

	return AosQueryColumn(mIILName, query_rslt, query_bitmap, query_context, rdata);
}


bool
AosTermHitType::collectInfo(const AosRundataPtr &rdata)
{
	OmnNotImplementedYet;
	return false;
	// if (mIILId)
	// {
	// 	return AosIILClient::getSelf()->collectInfo(mIILId, mCondInfo, rdata);
	// }
	// return AosIILClient::getSelf()->collectInfo(mIILName, mCondInfo, rdata);
}

