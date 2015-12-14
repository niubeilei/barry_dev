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
// 2013/12/17 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_Vector2D_Vector2D_h
#define Aos_Vector2D_Vector2D_h

#include "Vector2D/Ptrs.h"
//#include "StatUtil/Ptrs.h"
#include "SEInterfaces/Ptrs.h"
#include "SEInterfaces/StructProcObj.h"
#include "Rundata/Rundata.h"
#include "Rundata/Ptrs.h"
#include "SEUtil/StrValueInfo.h"
#include "Thread/Mutex.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/Buff.h"
#include "Util/Ptrs.h"
#include "StatUtil/StatTimeArea.h"
#include "Vector2DUtil/Vt2dInfo.h"
#include "Vector2DUtil/Vt2dModifyInfo.h"
#include "Vector2DUtil/Vt2dRecord.h"
#include "Vector2DQryRslt/Ptrs.h"

#include <map>

//#define AOS_VECTOR2D_DOCID 		"docid" 
//#define AOS_VECTOR2D_TIME 		"time" 
//#define AOS_VECTOR2D_STR 		"str" 

class AosStatCube;

class AosVector2D : public OmnRCObject
{
	OmnDefineRCObject;


	struct TimeBlockArea
	{
		u32						mTimeBlockId;
		vector<AosStatTimeArea>	mTimeAreas;
	};

public:
	//yang,2015/09/07
	enum E
	{
		NORMAL,
		MINMAX,
		DIST_COUNT
	};

private:
	E mMeasureType;

	u64					mStatId;
	u32					mCubeId;
	AosVt2dInfo			mVt2dInfo;	
	AosVector2DConnPtr	mVt2dConn;

	u64          		mReadTime1;
	u64         		mReadNum1;

	u64         		mReadTime2;
	u64         		mReadNum2;
	//AosMeasureValueDef	mValueDef;
	bool				mHasTimeField;

//yang

	OmnString mStatFunc;

	AosStatCube* mCube;
	OmnString mStatCubeKey;

	//yang
	AosVectorFile* mDistVecFile;
	AosVectorFile* mMinMaxVecFile;
	//vector<AosBuffPtr> mRcdOrigBuffs;
	
	//parsed time areas
	vector<AosStatTimeArea> mTimeAreas;

public:
	AosVector2D(const u64 stat_id, const u32 cube_id);
	~AosVector2D();

//rain
	void clearTimeAreas()
	{
		mTimeAreas.clear();
		return;
	}

//yang
	bool isDistCount()
	{
		return (mStatFunc=="dist_count") ? true : false;
	}
	bool isMinMax()
	{
		return (mStatFunc=="min") ? true : ((mStatFunc == "max") ? true :false);
	}
	void setStatCube(AosStatCube* cube)
	{
		mCube=cube;
	}

	u64 getDistCountNum();
	//u64 getMinMaxNum();

	E getMeasureType()
	{
		return mMeasureType;
	}


	//yang,the following four functions are for buffarray var record
	u64 getEncodedRecordFieldOffset(const char* data,OmnString dfname);
	u64 getRecordFieldLen(const char* data,OmnString dfname);
	AosBuffPtr decodeRecord(const char* data,int& orig_len);
	//u64 getOrigRecordMeasureFieldOffset(const char* data,OmnString dfname,u64 keylen);


	u32		getMeasureValueSize(){ return mVt2dInfo.mValueDef.mValueSize; };
	AosStatTimeUnit::E getTimeUnit(){ return mVt2dInfo.mGrpbyTimeUnit; };
	vector<MeasureInfo> * getMeasureInfos(){ return &mVt2dInfo.mMeasures; };
	bool	hasValidFlag() { return mVt2dInfo.mHasValidFlag; };

	bool 	config(
				const AosRundataPtr &rdata,
				const AosXmlTagPtr &conf);

	bool 	modify(AosRundata *rdata, 
				AosVt2dModifyInfo &mdf_info,
				const AosBuffPtr &buff);

	bool 	retrieve(
				const AosRundataPtr &rdata,
				vector<u64> &stat_docids,
				vector<AosStatTimeArea> &qry_time_areas,
				const AosVt2dQryRsltPtr &qry_rslt,
				const AosVt2dQryRsltProcPtr &qry_rslt_proc);

	bool 	retrieve(
				const AosRundataPtr &rdata,
				AosVt2dRecord *rcd,
				u64 sdocid,
				vector<AosStatTimeArea> &qry_time_areas);

	void outputCounters();
	void initCounters();

private:
	bool 	resolveQryTime(
				vector<AosStatTimeArea> &orig_time_areas,
				vector<AosStatTimeArea> &new_time_areas);

};
#endif

