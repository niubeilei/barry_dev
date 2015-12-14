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
// 2014/04/05	Created by Ketty
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_SEInterfaces_AggregationType_h
#define AOS_SEInterfaces_AggregationType_h

#include "Util/Ptrs.h"
#include "Util/String.h"

#define	 AOS_AGR_SUM		"sum"
#define	 AOS_AGR_AGGR_CONCAT "aggr_concat"
#define	 AOS_AGR_COUNT		"count"
#define	 AOS_AGR_MAX		"max"
#define	 AOS_AGR_MIN		"min"
#define	 AOS_AGR_SET		"set"
#define	 AOS_AGR_ACCU_COUNT	"accu_count"
#define	 AOS_AGR_ACCU_SUM	"accu_sum"
#define	 AOS_AGR_ACCU_MAX	"accu_max"
#define	 AOS_AGR_ACCU_MIN	"accu_min"
#define	 AOS_AGR_DIST_COUNT	"dist_count"
#define	 AOS_AGR_DIST_COUNT_QUERY	"dist_count_query"
#define	 AOS_AGR_DIST_COUNT_CREATE 	"dist_count_create"

class AosAggregationType 
{

public:
	enum E 
	{
		eInvalid,

		eSum,
		eCount,
		eMax,
		eMin,
		eSet,
		eAccuCount,
		eAccuSum,
		eAccuMax,
		eAccuMin,
		eDistCount,
		eDistCountQuery,
		eDistCountCreate,
		eAggr_concat,

		eInvalidMax
	};

	static bool isValid(const E tp)
	{
		return tp > eInvalid && tp < eInvalidMax;
	}

	static E toEnum(const OmnString &name);
	static bool addName(const OmnString &name, const E code);
	static OmnString toStr(const E cond);

};
#endif
