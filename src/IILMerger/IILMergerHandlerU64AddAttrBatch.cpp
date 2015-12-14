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
#include "IILMerger/IILMergerHandlerU64AddAttrBatch.h"


AosIILMergerHandlerU64AddAttrBatch::AosIILMergerHandlerU64AddAttrBatch(
		const AosCompareFunPtr &comp, 
		const bool stable,
		const u64 iilid,
		const u32 siteid,
		const bool value_unique,
		const bool docid_unique)
:
AosIILMergerHandler(iilid, siteid, comp, stable),
mValueUnique(value_unique),
mDocidUnique(docid_unique)
{
};


AosIILMergerHandlerU64AddAttrBatch::~AosIILMergerHandlerU64AddAttrBatch()
{
};


bool
AosIILMergerHandlerU64AddAttrBatch::proc(
		const AosBuffPtr &buff,
		const AosRundataPtr &rdata)
{
	//AosIILType type;
	//AosIILPtr iil = AosIILMgr::getSelf()->loadIILByIDPublic(
	//	mIILID, mSiteid, type, false, false, rdata);
	//aos_assert_r(iil && type == eAosIILType_U64, false);

	//u64 *entries = (u64 *)buff->data();
	//int size = mComp->size;
	//int num = buff->dataLen() / size; 
		
	//AosIILU64Ptr iilu64 = (AosIILU64*)iil.getPtr();
	//bool rslt = iilu64->addBlockSafe(entries, num, false, rdata);
	//AosIILMgr::getSelf()->returnIILPublic(iil, rdata);
	return true;
};
