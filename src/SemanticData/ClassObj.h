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
// The super class for all semantics data. 
//
// Modification History:
// 11/27/2007: Created by Chen Ding
// 
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SemanticData_ClassObj_h
#define Aos_SemanticData_ClassObj_h

#include "aosUtil/Types.h"
#include "SemanticData/Var.h"
#include "SemanticData/Types.h"
#include "Util/RCObjImp.h"
#include <list>


class AosClassObj : public AosVar
{
	OmnDefineRCObject;

	struct InstCount
	{
		u16		mCount;
		u32		mSec;
	};

private:
	std::string		mClassName;
	u32				mLastReadTimestamp;
	u32				mLastModifyTimestamp;
	u32				mLastInstCreationTimestamp;
	u32				mLastInstDeletionTimestamp;
	u32				mLastResetTimestamp;
	u32				mLastMarkerTimestamp;
	u32				mLastCounterTimestamp;
	u32				mLastPatTimestamp;
	std::list<InstCount>	mInstStatList;

public:
	AosClassObj(const std::string &name);
	~AosClassObj();

	int64_t 	getInstCreated(const int time_min, 
						   	const int time_max,
						   	const AosTimeMeasure::E time_measure);
	int64_t 	getInstCreated(const u32 startTime, 
							const u32 endTime);
	std::string	getName() const {return mClassName;}

};
#endif

