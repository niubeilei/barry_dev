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
// 2014/04/02	Created by Ketty 
// 
////////////////////////////////////////////////////////////////////////////

#ifndef StatUtil_StatInternalDoc_h
#define StatUtil_StatInternalDoc_h

#include "StatUtil/StatDefineDoc.h"
#include "Vector2DUtil/Vt2dInfo.h"
#include "XmlUtil/Ptrs.h"
#include "Util/String.h"
#include "alarm_c/alarm.h"

#include <vector>
using namespace std;

class AosStatInternalDoc 
{

private:
	OmnString		mCrtObjid;
	OmnString		mStatIdentifyKey;

	AosXmlTagPtr	mKeyFieldsConf;
	AosXmlTagPtr	mKeyConnConf;
	AosXmlTagPtr	mTimeFieldConf;
	AosXmlTagPtr	mCondConf;
	AosXmlTagPtr	mShuffleFieldsConf;

	vector<AosVt2dInfo>	mVt2dInfos;
	vector<AosXmlTagPtr> mVt2dConnConfs;
	vector<AosXmlTagPtr> mStatCubeConfs;

public:
	AosStatInternalDoc(const OmnString &stat_identify_key);
	~AosStatInternalDoc();
	
	bool	config(const AosRundataPtr &rdata, const AosXmlTagPtr &conf);
	
	bool 	addStatDefDoc(
				const AosRundataPtr &rdata,
				AosStatDefineDoc &def_doc);

	OmnString toXmlString(const AosRundataPtr &rdata);

private:
	bool 	measureExist(
				const OmnString &measure_name,
				AosStatTimeUnit::E time_unit);

	AosXmlTagPtr getVt2dConnConf(
				const AosRundataPtr &rdata,
				AosStatDefineDoc &def_doc,
				const AosStatTimeUnit::E time_unit,
				const OmnString &time_fname);

	bool 	addNewVt2dInfo(
				const AosRundataPtr &rdata,
				const AosXmlTagPtr &vt2d_conn_conf,
				const AosStatTimeUnit::E grpby_time_unit,
				const OmnString &time_fname,
				vector<MeasureInfo>	&new_m_infos);

	bool 	initStatCubeConfs(const AosRundataPtr &rdata);

	u64 	createStatMetaFile(
				const AosRundataPtr &rdata,
				const u32 cube_id,
				const OmnString &file_prefix);

};

#endif
