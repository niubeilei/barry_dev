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
// 09/26/2009	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_DataStructs_StatTimeCond_h
#define AOS_DataStructs_StatTimeCond_h

#include "Rundata/Ptrs.h"
#include "XmlUtil/Ptrs.h"
#include "Util/Ptrs.h"
#include "Util/Buff.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "StatUtil/StatUtil.h"


class AosStatTimeCond : virtual public OmnRCObject
{
	OmnDefineRCObject;

public:
	struct GrpbyTimeArea 
	{
		u64 	start;
		u64 	end;
		u64		grpby_time_value;
	};

private:
	u64					mSlot;
	vector<AosStatTimeArea>	mTimeAreas;
	vector<GrpbyTimeArea>	mGrpbyTimeAreas;	

public:
	AosStatTimeCond(const u64 &slot)
	:
	mSlot(slot)
	{
	}

	AosStatTimeCond(){};
	~AosStatTimeCond(){};
	
	void appendTimeArea(const u64 start, const u64 end)
	{
		AosStatTimeArea area;
		area.start_time = start;
		area.end_time = end;
		mTimeAreas.push_back(area);
	}
	
	vector<AosStatTimeArea> & getTimeAreas(){ return mTimeAreas; };
	
	void appendGroupbyTimeArea(const u64 grpby_time_value, const u64 start, const u64 end)
	{
		GrpbyTimeArea area;
		area.start = start;
		area.end = end;
		area.grpby_time_value = grpby_time_value;
	
		mGrpbyTimeAreas.push_back(area);
	}
	
	void clear(){ mSlot = 0; mTimeAreas.clear(); mGrpbyTimeAreas.clear(); };

	u64 getSlot()const {return mSlot;}

	u32 getGroupbyAreaNum() const {return mGrpbyTimeAreas.size();}

	bool getGroupbyArea(const u32 idx, u64 &start, u64 &end, u64 &grpby_time_value)  const 
	{
		start = end = 0;
		aos_assert_r(idx < mGrpbyTimeAreas.size(), false);

		start = mGrpbyTimeAreas[idx].start;
		end = mGrpbyTimeAreas[idx].end;
		grpby_time_value = mGrpbyTimeAreas[idx].grpby_time_value;
		
		aos_assert_r(start <= end, false);
		return true;
	}
};


/*
class AosStatTimeCond : virtual public OmnRCObject
{
	OmnDefineRCObject;

	struct Cond
	{
		u64 	start;
		u64 	end;
	};

private:
	u64					mSlot;
	u32					mCrtIdx;
	vector<Cond>		mCond;	
	u32					mNum;
public:
	AosStatTimeCond(const u64 &slot)
	:
	mSlot(slot),
	mCrtIdx(0),
	mNum(0)
	{
	}

	AosStatTimeCond(){};
	~AosStatTimeCond(){};
	
	void appendStart(const u32 start)
	{
		mCrtIdx = mCond.size();
		Cond cond;
		cond.start = start;
		cond.end = 0;
		mCond.push_back(cond);
		mNum++;
	}

	void appendEnd(const u32 end)
	{
		mCond[mCrtIdx].end = end;
	}

	u64 getSlot()const {return mSlot;}

	u32 getNumCond() const {return mNum;}

	bool getTimeCond(const u32 idx, u64 &start, u64 &end)  const 
	{
		start = end = 0;
		aos_assert_r(idx < mCond.size(), false);
		start = mCond[idx].start;
		end = mCond[idx].end;
		aos_assert_r(start <= end, false);
		return true;
	}
};
*/

#endif
