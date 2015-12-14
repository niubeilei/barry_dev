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
// 2012/09/10 Created by Ken Lee
////////////////////////////////////////////////////////////////////////////
#include "IILMerger/IILMergerHandlerStrAddAttrBatch.h"


AosIILMergerHandlerStrAddAttrBatch::AosIILMergerHandlerStrAddAttrBatch(
		const AosCompareFunPtr &comp, 
		const bool stable,
		const u64 iilid,
		const u32 siteid,
		const bool value_unique,
		const bool docid_unique,
		const AosOrder::E order)
:
AosIILMergerHandler(iilid, siteid, comp, stable),
mValueUnique(value_unique),
mDocidUnique(docid_unique),
mOrder(order)
{
};


AosIILMergerHandlerStrAddAttrBatch::~AosIILMergerHandlerStrAddAttrBatch()
{
};


bool
AosIILMergerHandlerStrAddAttrBatch::proc(
		const AosBuffPtr &buff,
		const AosRundataPtr &rdata)
{
	//AosIILType type;
	//AosIILPtr iil = AosIILMgr::getSelf()->loadIILByIDPublic(
	//	mIILID, mSiteid, type, false, false, rdata);
	//aos_assert_r(iil && type == eAosIILType_Str, false);

	//char *entries = buff->data();
	//int size = mComp->size;
	//int num = buff->dataLen() / size; 
		
	//AosIILStrPtr iilstr = (AosIILStr*)iil.getPtr();
	//bool rslt = iilstr->addBlockSafe(entries, size, num, false, rdata);
	//AosIILMgr::getSelf()->returnIILPublic(iil, rdata);
	return true;
};
