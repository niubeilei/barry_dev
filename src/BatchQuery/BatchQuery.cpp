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
//
// Modification History:
// 03/11/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "BatchQuery/BatchQuery.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "BatchQuery/BatchQueryCtlr.h"
#include "SEInterfaces/BitmapObj.h"


AosBatchQuery::AosBatchQuery(
		const AosBatchQueryCtlrPtr &ctlr, 
		const AosBitmapObjPtr &bitmap,
		const u32 size,
		const AosRundataPtr &rdata)
:
mCtlr(ctlr),
mBitmap(bitmap),
mSize(size),
mStartIdx(-1),
mEndIdx(-1)
{
}


AosBatchQuery::~AosBatchQuery()
{
}


bool
AosBatchQuery::process(const AosRundataPtr &rdata)
{
	// This function starts procesing the batch query. The batch query
	// has a number of IILs. For each IIL, it retrieves all the matched
	// docids and sets them to the bitmap. For the first IIL, if 
	// mStartIdx is negative, it starts from the beginning. Otherwise, 
	// it is the index to start with. For the last IIL, if mEndIdx 
	// is negative, it ends at the end. Otherwise, it ends at the position
	// pointed to by mEndIdx.
	// After finishing, it reports to mCtlr. 
	OmnNotImplementedYet;
	return false;
}

