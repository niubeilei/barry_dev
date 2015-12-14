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
#ifndef AOS_StatUtil_StatQueryInfo_h
#define AOS_StatUtil_StatQueryInfo_h

#include "StatUtil/Ptrs.h"
#include "StatUtil/QueryMeasureInfo.h"
#include "StatUtil/StatTimeArea.h"
#include "StatUtil/StatMeasureField.h"
#include "StatUtil/StatTimeField.h"
#include "StatUtil/StatKeyField.h"
#include "StatUtil/StatCond.h"
#include "JQLStatement/Ptrs.h"
#include "JQLStatement/JqlHaving.h"

//#include <vector>
//using namespace std;


struct AosStatQueryInfo
{
	OmnString							mStatName;
	OmnString							mTimeKeyField;
	vector<OmnString>					mStatKeyFields;
	//arvin 2015.09.03
	map<OmnString,int>					mKeyIdxMap;
	map<int,AosDataType::E>				mKeyIdxTypeMap;
	
	bool								mNeedFullScan;

	u32									mPageSize;
	u64									mPageOffset;
	vector<AosStatFieldPtr>				mQryMeasures;
	vector<AosStatCond> 				mQryKeyConds;
	OmnString							mHavingCond;
	bool								mHitShuffleFields;
	vector<AosStatTimeArea>				mQryTimeConds;
	vector<AosStatFieldPtr>				mGrpbyFields;
	vector<AosStatFieldPtr>				mCubeGrpbyFields;
	vector<vector<AosStatFieldPtr> > 	mRollupGrpbyFieldsGrp;
	vector<AosStatFieldPtr>				mOrderbyFields;
	vector<bool>						mOrderbyAscs;
	AosJqlHavingPtr         			mHavingPtr;
	
	AosStatTimeUnit::E 	 				mGrpbyTimeUnit;
	AosStatTimeUnit::E 	 				mStatTimeUnit;  //use to save statistic model timeUnit
	
	AosStatQueryInfo();
	

	bool 	init(const AosRundataPtr &rdata,
				const u32	page_size,	
				const u64	page_offset,
				vector<AosStatCond> &stat_conds,
				vector<AosStatFieldPtr> &grpby_fields,
				vector<AosStatFieldPtr> &cube_grpby_fields,
				vector<vector<AosStatFieldPtr> > &rollup_grpby_fields_grp,
				vector<AosStatFieldPtr> &orderby_fields,
				vector<bool> &order_ascs,
				const OmnString &having_conds,
				const bool hitShuffleFields = false);

	bool	initStat(const AosRundataPtr &rdata,
				const AosStatisticPtr &stat);
	
	bool	addQryMeasure(const AosStatFieldPtr &field);

	bool	config(const AosXmlTagPtr &conf);

	bool 	toXmlString(OmnString &str);
	
	OmnString toXmlString();

};

#endif
