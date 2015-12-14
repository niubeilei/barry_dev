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
// handle the SEServer send request to MsgServer 
//
// Modification History:
// 08/28/2011	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_API_ApiB_h
#define AOS_API_ApiB_h

#include "Debug/Debug.h"
#include "Rundata/Ptrs.h"
#include "SEInterfaces/Ptrs.h"
#include "SEUtil/SeTypes.h"


class AosRundata;

extern int AosBinarySearchOnU32Array(
		AosRundata *rdata, 
		const u32 *array, 
		const int size, 
		const u32 value);

extern bool AosBitmapQueryColumn(
		const OmnString &iilname,
		const AosQueryRsltObjPtr &query_rslt,
		const AosBitmapObjPtr &query_bitmap,
		const AosQueryContextObjPtr &query_context,
		const AosRundataPtr &rdata);

extern bool AosBitmapRsltQueryColumn(
		const OmnString &iilname,
		const AosQueryRsltObjPtr &query_rslt,
		const AosBitmapObjPtr &query_bitmap,
		const AosQueryContextObjPtr &query_context,
		const AosRundataPtr &rdata);

extern bool AosBatchAdd(
		const OmnString &iilname,
		const AosIILType &iiltype,
		const int entry_len,
		const AosBuffPtr &buff,
		const u64 &executor_id,
		const u64 &snap_id,
		const u64 &task_docid,
		const AosRundataPtr &rdata);

extern bool AosBatchDel(
		const OmnString &iilname,
		const AosIILType &iiltype,
		const int entry_len,
		const AosBuffPtr &buff,
		const u64 &executor_id,
		const u64 &snap_id,
		const u64 &task_docid,
		const AosRundataPtr &rdata);

#endif
