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
// 2014/01/13 Created by Ken Lee
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_DataStructs_StatReaderRequest_h
#define AOS_DataStructs_StatReaderRequest_h

#include "DataStructs/StatTimeCond.h"
#include "TransBasic/Trans.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/Ptrs.h"
#include "StatUtil/Ptrs.h"
#include "StatUtil/Vector2DGrpbyRslt.h"
//#include "StatUtil/StatGroupByTime.h"
//#include "StatUtil/StatGroupByKey.h"
//#include "StatUtil/StatRecordParser.h"


class AosStatReaderRequest : virtual public OmnRCObject
{
	OmnDefineRCObject;

public:
	u64             				mDistBlockDocid;
	vector<u64>     				mStatIds;
	vector<AosStatTimeCond *> 		mTimeConds;
	AosStatTimeUnit::E				mGrpbyTimeUnit;
	AosVector2DGrpbyRsltPtr 		mGrpByRslt;

	AosStatReaderRequest(
			const u64 did,
			vector<AosStatTimeCond *> &time_conds,
			const AosStatTimeUnit::E grpby_time_unit,
			const AosVector2DGrpbyRsltPtr &grpby_rslt)
	:
	mDistBlockDocid(did),
	mTimeConds(time_conds),
	mGrpbyTimeUnit(grpby_time_unit),
	mGrpByRslt(grpby_rslt)
	{
	}

	~AosStatReaderRequest()
	{
	}

	void appendEntry(const u64 &stat_id)
	{
		mStatIds.push_back(stat_id);
	}
};

#endif

