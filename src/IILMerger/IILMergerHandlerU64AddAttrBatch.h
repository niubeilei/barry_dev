////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// This type of IIL maintains a list of (string, docid) and is sorted
// based on the string value. 
//
// Modification History:
// 04/09/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_IILMerger_IILMergerHandlerU64AddAttrBatch_h
#define AOS_IILMerger_IILMergerHandlerU64AddAttrBatch_h

#include "IILMerger/IILMergerHandler.h"
#include "Rundata/Rundata.h"
#include "Rundata/Ptrs.h"
#include "Util/Ptrs.h"
#include "Util/BuffArray.h"
#include "Util/StrU64Array.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"


class AosIILMergerHandlerU64AddAttrBatch : virtual public AosIILMergerHandler
{
public:
	bool		mValueUnique;
	bool		mDocidUnique;

public:
	AosIILMergerHandlerU64AddAttrBatch(
			const AosCompareFunPtr &comp, 
			const bool stable,
			const u64 iilid,
			const u32 siteid,
			const bool value_unique,
			const bool docid_unique);
	~AosIILMergerHandlerU64AddAttrBatch();

	virtual bool	proc(
						const AosBuffPtr &buff,
						const AosRundataPtr &rdata);
};
#endif

