////////////////////////////////////////////////////////////////////////////
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

#ifndef StatQueryAnalyzer_StatQueryUnit_h
#define StatQueryAnalyzer_StatQueryUnit_h

#include "Rundata/Ptrs.h"
#include "SEInterfaces/Ptrs.h"
#include "Util/String.h"
#include "StatUtil/StatUtil.h"
#include "StatUtil/Ptrs.h"
#include "StatUtil/InternalStat.h"
#include "StatUtil/StatRecord.h"
#include "StatUtil/StatMeasure.h"
#include "StatUtil/StatDefineDoc.h"
#include "StatQueryAnalyzer/Ptrs.h"
#include "Util/Ptrs.h"
#include "QueryUtil/QrUtil.h"

#include <vector>
using namespace std;

class AosStatQueryUnit: public OmnRCObject 
{
	OmnDefineRCObject;

private:
	AosStatDefineDocPtr				mStatDef;
	vector<AosQrUtil::FieldDef> 	mSelectedFields;
	vector<AosStatMeasure>			mMeasures;
	vector<AosStatTimeArea> 	mStatQryConds;
	vector<OmnString>				mGroupByKeyFields;
	AosStatTimeUnit::E				mGroupByTimeUnit;
	vector<OmnString>				mStatKeys;

	AosDatasetObjPtr				mStatDataset;

public:
	AosStatQueryUnit(
			const AosStatDefineDocPtr &stat_def,
			vector<AosQrUtil::FieldDef> &select_fields);
	~AosStatQueryUnit();
	
	AosStatDefineDocPtr getStatDefineDoc(){ return mStatDef; };
	vector<OmnString> & getGroupByFields(){ return mGroupByKeyFields; };
	vector<AosQrUtil::FieldDef> & getSelectFields(){return mSelectedFields;}
	
	bool 	initMeasureInfo(
				set<AosStatMeasure> &measure,
				vector<OmnString> &group_by_fields);
	bool	initStatKeyFields(AosInternalStat &internal_stat);
	bool 	initStatQryTimeConds(
				const AosRundataPtr &rdata,
				vector<AosQueryTermObjPtr> &cond_terms);
	bool 	initStatQryValueConds(
				const AosRundataPtr &rdata,
				vector<AosQueryTermObjPtr> &cond_terms);
	bool 	initStatDataset(
				const AosRundataPtr &rdata,
				const u32 docid_num,
				const AosBuffPtr &docids_buff);


	bool 	run(const AosRundataPtr &rdata);

	bool 	generateRslt(const AosRundataPtr &rdata, OmnString &contents);

private:
	bool 	resolveGrpbyFieldIdx();
	bool 	resolveSelectedFieldIdx();

	AosXmlTagPtr generateDatasetXml(
				const AosRundataPtr &rdata,
				const u32 docid_num,
				const AosBuffPtr &docids_buff);
	OmnString createSchemaConfStr(const AosRundataPtr &rdata);
	OmnString createStatConfStr(const AosRundataPtr &rdata);

	bool 	generateEachEntry(
				const AosRundataPtr &rdata,
				AosDataRecordObj *data_rcd,
				OmnString &rcd_str);

};

#endif
