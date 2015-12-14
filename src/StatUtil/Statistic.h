///////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: Statistics.h
// Description:
//   
//
// Modification History:
// 2014/01/22 Created by Ketty
// 
////////////////////////////////////////////////////////////////////////////

#ifndef StatUtil_Statistic_h
#define StatUtil_Statistic_h

#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "XmlUtil/Ptrs.h"
//#include "StatUtil/QueryMeasureInfo.h"
#include "StatUtil/MeasureInfo.h"
//#include "StatUtil/StatModifyInfo.h"
//#include "StatUtil/StatQueryInfo.h"
#include "StatUtil/Ptrs.h"
#include "StatServer/Ptrs.h"
#include "Vector2DUtil/Vt2dInfo.h"
#include "Vector2DUtil/Vt2dModifyInfo.h"
#include "Thread/Ptrs.h"

#include <vector>
#include <map>
using namespace std;

class AosStatistic: public OmnRCObject
{
	OmnDefineRCObject;
	
private:
	//OmnMutexPtr		mLock;
	u64					mStatId;
	OmnString			mStatName;
	OmnString			mIdentifyKey;
	AosXmlTagPtr		mCubeConfs;
	
	OmnString 			mTimeUnit;
	OmnString			mTimeField;
	vector<OmnString>	mKeyFields;
	vector<OmnString>	mShuffleFields;
	vector<MeasureInfo>	mMeasureInfos;
	vector<AosVt2dInfo>	mVt2dInfos;

public:
	AosStatistic();
	~AosStatistic();
	
	static AosStatisticPtr retrieveStat(
				const AosRundataPtr &rdata,
				const AosXmlTagPtr &stat_qry_conf);

	static AosStatisticPtr retrieveStat(
				const AosRundataPtr &rdata,
				const OmnString &stat_name);

	static AosXmlTagPtr refactorInternalStatConf(
				const AosRundataPtr &rdata,
				const OmnString &stat_doc_objid,
				const AosXmlTagPtr &stat_def_doc);

	bool 	config(
				const AosRundataPtr &rdata,
				const AosXmlTagPtr &conf);

	int 	getVt2dIdxByMeasureName(const OmnString &name,
				const AosStatTimeUnit::E grpby_time_unit);
	
	//yang
	int 	getVt2dIdxByMeasureIdx(const int idx,
				const AosStatTimeUnit::E grpby_time_unit);

	int 	getVt2dIdxByMeasureName(const OmnString &name);
	
	//yang
	int 	getVt2dIdxByMeasureIdx(const int idx);


	bool	getMeasureInfo(
				const u32 vt2d_idx,
				const u32 vt2d_measure_idx,
				MeasureInfo &m_info);
	
	AosXmlTagPtr getStatCubesConf(){ return mCubeConfs; };

	u64		getStatId(){ return mStatId; };
	
	OmnString getStatName(){ return mStatName; };
	
	vector<OmnString> & getKeyFields(){ return mKeyFields; };
	
	vector<OmnString> & getShuffleFields(){ return mShuffleFields; };

	bool 	getVt2dInfo(const u32 vt2d_idx, AosVt2dInfo &info);
	
	OmnString	getTimeFieldName(){ return mTimeField; };
	
	OmnString	getTimeUnit(){ return mTimeUnit; };
	
	OmnString	getIdentifyKey(){ return mIdentifyKey; };
	
	u32		getStatKeyNum(){ return mKeyFields.size(); };
	
	bool 	isMeasureRawField(const OmnString &fname);
	
	OmnString findKeyFieldByMeasureField(const OmnString &measure_field);
	
	bool	isContainKeyField(const OmnString &field);
	
	bool	isContainMeasure(const OmnString &measure_name,
				const OmnString &field_name,
				const OmnString &aggr_func_str);
	bool	isContainJoinCond(const OmnString &cond_str);

	bool 	isContainTimeUnit(const AosStatTimeUnit::E grpby_time_unit);
	
	int 	pickVt2dIdx(const OmnString &name, AosStatTimeUnit::E grpby_time_unit);
	
	bool	isContainTimeField(const OmnString &fname){ return fname == mTimeField; };

	bool	isContainTimeField(const OmnString &fname, const AosStatTimeUnit::E time_unit);
	
	bool	isContainMeasure(const OmnString &measure_name);
	
	bool	hasDistCountMeasure();
	
	AosDataType::E getMeasureDataType(const OmnString &name);
	
	AosDataType::E getMeasureType(const OmnString &name);
	
	u32 getMeasureSize();
	
	bool getMeasureFields(vector<OmnString> &measreFields); 
private:
	static AosXmlTagPtr getInternalStatConf(
				const AosRundataPtr &rdata,
				const AosXmlTagPtr &stat_doc,
				const OmnString &stat_identify_key);

	static bool addNewInternalDocConf(
				const AosRundataPtr &rdata,
				const AosXmlTagPtr &stat_doc,
				const OmnString &stat_name,
				const OmnString &stat_identify_key,
				AosXmlTagPtr &new_intedoc_conf);

	bool 	configTimeField(
				const AosRundataPtr &rdata, 
				const AosXmlTagPtr &conf);

	bool 	configKeyFields(
				const AosRundataPtr &rdata, 
				const AosXmlTagPtr &conf);

	bool 	configVt2dInfos(
				const AosRundataPtr &rdata,
				const AosXmlTagPtr &conf);
	
	bool	configShuffleFields(
				const AosRundataPtr &rdata, 
				const AosXmlTagPtr &conf);

};

#endif
