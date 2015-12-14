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
#ifndef Aos_SEInterfaces_StatisticsObj_h
#define Aos_SEInterfaces_StatisticsObj_h

#include "Jimo/Jimo.h"
#include "SEInterfaces/Ptrs.h"
#include "Util/Opr.h"


class AosStatisticsObj : public AosJimo
{
private:

public:
	AosStatisticsObj(const int version);
	virtual ~AosStatisticsObj();

	static AosStatisticsObjPtr createStatisticsStatic(
					AosRundata *rdata, 
					const AosXmlTagPtr &def);

	virtual vector<AosStatModelObjPtr> getStatModels(AosRundata *rdata) = 0;

	virtual AosStatModelObjPtr getStatModel(
					AosRundata *rdata, 
					const OmnString &name) = 0;

	virtual AosStatisticsObjPtr createStatistics(
					AosRundata *rdata, 
					const AosXmlTagPtr &def) = 0;

	virtual bool addStatModel(
					AosRundata *rdata, 
					const OmnString &name,
					const vector<OmnString> &keys,
					const OmnString &file_fname, 
					const vector<AosStatMeasureObjPtr> &measures) = 0;

	virtual bool removeStatModel(
					AosRundata *rdata, 
					const OmnString &name) = 0;
};

#endif
