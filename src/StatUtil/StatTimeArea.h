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
// 2014/02/18 Created by Ketty 
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_StatUtil_StatTimeArea_h
#define Aos_StatUtil_StatTimeArea_h

#include "Util/Ptrs.h"
#include "Util/ValueRslt.h"
#include "Util/Buff.h"
#include "StatUtil/StatTimeUnit.h"
#include "JQLExpr/ExprGenFunc.h"

struct AosStatTimeArea
{
	int64_t		start_time;
	int64_t 	end_time;
	AosStatTimeUnit::E time_unit;

public:
	AosStatTimeArea();
	AosStatTimeArea(i64 s_time, i64 e_time, AosStatTimeUnit::E t_unit);

	OmnString toXmlString();
	
	bool config(const AosXmlTagPtr &conf);
	
	//bool	serializeTo(const AosBuffPtr &buff)
	
	//bool	serializeFrom(const AosBuffPtr &buff)

	static bool	parseTimeCond(
				const AosRundataPtr &rdata,
				AosStatTimeUnit::E &time_unit,
				const AosOpr &cond_opr,
				const OmnString &time_str,
				vector<AosStatTimeArea> &time_areas);

private:
	static bool intersectTimeCondByAnd(
				vector<AosStatTimeArea> &rslt_conds,
				AosStatTimeArea &new_cond);

	static bool intersectTimeCondByOr(
				vector<AosStatTimeArea> &rslt_conds,
				vector<AosStatTimeArea> &or_conds);

	static bool intersectTimeCondByAndPriv(
				vector<AosStatTimeArea> &rslt_conds,
				AosStatTimeArea &new_cond);

};

#endif

