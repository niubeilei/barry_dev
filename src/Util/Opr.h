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
// 01/05/2010 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_Util_Opr_h
#define Aos_Util_Opr_h

#include "Util/String.h"

#define AOSQUERYTAG_TIMED_CID		"10"
#define AOSQUERYTAG_RANGE_GE_LE		"r1"
#define AOSQUERYTAG_RANGE_GE_LT		"r2"
#define AOSQUERYTAG_RANGE_GT_LE		"r3"
#define AOSQUERYTAG_RANGE_GT_LT		"r4"

enum AosOpr
{
	eAosOpr_Invalid,

	eAosOpr_gt,			// >
	eAosOpr_ge,			// >=
	eAosOpr_eq,			// ==
	eAosOpr_lt,			// <
	eAosOpr_le,			// <=
	eAosOpr_ne,			// !=
	eAosOpr_an,			// Matching any
	eAosOpr_Objid,		// Search based on objid
	eAosOpr_prefix,		// all entries with a given Prefix
	eAosOpr_like,		//ken 2011/5/25
	eAosOpr_not_like,	//andy 2014/11/18
	eAosOpr_null,	//andy 2014/11/18
	eAosOpr_not_null,	//andy 2014/11/18
	eAosOpr_date,		// Chen Ding, 08/08/2011
	eAosOpr_epoch,		// Chen Ding, 08/08/2011
	eAosOpr_range_ge_le,
	eAosOpr_range_ge_lt,
	eAosOpr_range_gt_le,
	eAosOpr_range_gt_lt,
	eAosOpr_in,			// Shawn, 08/12/2014
	eAosOpr_not_in,		// Barry Niu, 02/12/2014
	eAosOpr_distinct,	// Shawn, 10/16/2014

	eAosOpr_max
};


extern AosOpr AosOpr_toEnum(const OmnString &str);
extern OmnString AosOpr_toStr(const AosOpr code);
extern OmnString AosOpr_toStr2(const OmnString opr);
extern OmnString AosOpr_toStrCode(const OmnString &str);
inline bool AosOpr_valid(const AosOpr opr)
{
	return (opr >= eAosOpr_gt && opr <= eAosOpr_max);
}
inline bool AosOpr_isRangeOpr(const AosOpr opr)
{
	return (opr >= eAosOpr_range_ge_le && opr <= eAosOpr_range_gt_lt);
}

#endif

