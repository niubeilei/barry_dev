////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// Modification History:
// 2014/07/25 Created by Ketty 
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_Vector2DUtil_Vt2dInfo_h
#define AOS_Vector2DUtil_Vt2dInfo_h

#include "StatUtil/MeasureInfo.h"
#include "StatUtil/StatTimeUnit.h"
#include "Vector2DUtil/MeasureValueDef.h"
#include "XmlUtil/Ptrs.h"
#include "XmlUtil/XmlTag.h"

struct AosVt2dInfo
{
	OmnString		mVt2dName;
	OmnString		mTimeFieldName;
	AosStatTimeUnit::E	mGrpbyTimeUnit;
	bool			mHasValidFlag;
	vector<MeasureInfo> mMeasures;
	AosMeasureValueDef	mValueDef;
	
	AosVt2dInfo();

	int getMeasureIdx(const OmnString &name);
	
	void initVt2dValueDef(AosMeasureValueDef &value_def);

	bool config(const AosRundataPtr &rdata, const AosXmlTagPtr &conf);
	
	OmnString toXmlString(const AosRundataPtr &rdata);

	u64 getMeasureNum()
	{
		return mMeasures.size();
	}
	
	u64 getDistCountMeasureNum();
	/*
//yang
	OmnString getStatType()
	{
		return mMeasures[0].mAggrFuncStr;
	}
	*/

};


#endif
