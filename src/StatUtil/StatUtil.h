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
#ifndef Aos_StatUtil_StatUtil_h
#define Aos_StatUtil_StatUtil_h

#include "Util/Ptrs.h"
#include "Util/ValueRslt.h"
#include "Util/Buff.h"
#include "StatUtil/StatTimeUnit.h"
#include "JQLExpr/ExprGenFunc.h"

class AosStatUtil
{
public:
	
	static bool include();

	static bool setValueRsltToBuff(
			AosRundata *rdata,
			const AosValueRslt &value,
			const AosDataType::E data_type,
			const AosBuffPtr &buff);
	
	static bool setStatValueToBuff(
			const AosRundataPtr &rdata,
			char * value,
			const AosDataType::E data_type,
			const AosBuffPtr &buff);
	
	static bool setStatValueToValueRslt(
			const AosRundataPtr &rdata,
			char * value,
			const AosDataType::E data_type,
			AosValueRslt &value_rslt);


	static bool setStatValueToStr(
			const AosRundataPtr &rdata,
			const AosBuffPtr &value,
			const AosDataType::E data_type,
			OmnString &content);

	//static bool getStatValue(
	//		char *array,
	//		const int64_t data_len,
	//		const int pos,
	//		AosDataType::E data_type,
	//		const AosBuffPtr &buff);

	static bool setInt64ToBuff(
			const AosRundataPtr &rdata,
			const int64_t &vv,
			const AosDataType::E data_type,
			const AosBuffPtr &buff);

	//static bool checkIsCountRecds(
	//		const AosRundataPtr &rdata,
	//		AosExprObj* expr);

	static int getSvrIdByStatid(const u64 stat_id, const int docs_per_dist);

};

#endif

