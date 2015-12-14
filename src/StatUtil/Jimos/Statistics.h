////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
//
// Modification History:
// 2015/02/06 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_StatUtil_Statistics_h
#define Aos_StatUtil_Statistics_h

#include "Jimo/Jimo.h"
#include "SEInterfaces/StatisticsObj.h"
#include "StatUtil/Jimos/Ptrs.h"


class AosStatistics : public AosStatisticsObj
{
	OmnDefineRCObject;

private:
	OmnString mTableName;
	vector<AosStatModelObjPtr> mStatModels;

public:
	AosStatistics(const int version);
	virtual ~AosStatistics();

	// Jimo Interface
	virtual AosJimoPtr cloneJimo() const;

	virtual vector<AosStatModelObjPtr> getStatModels(AosRundata *rdata);

	virtual AosStatModelObjPtr getStatModel(
					AosRundata *rdata, 
					const OmnString &name);

	virtual AosStatisticsObjPtr createStatistics(
					AosRundata *rdata, 
					const AosXmlTagPtr &def);

	virtual bool addStatModel(
					AosRundata *rdata, 
					const OmnString &name,
					const vector<OmnString> &keys,
					const OmnString &file_fname, 
					const vector<AosStatMeasureObjPtr> &measures);

	virtual bool removeStatModel(
					AosRundata *rdata, 
					const OmnString &name);

};

#endif
