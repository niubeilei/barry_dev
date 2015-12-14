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

#ifndef StatQueryPattern1_StatQueryPattern1_h
#define StatQueryPattern1_StatQueryPattern1_h

#include "Rundata/Ptrs.h"
#include "SEInterfaces/Ptrs.h"
#include "Util/String.h"
#include "XmlUtil/Ptrs.h"
#include "StatUtil/StatUtil.h"
#include "QueryUtil/QrUtil.h"
#include "StatUtil/StatMeasure.h"
#include "StatUtil/Ptrs.h"
#include "StatQueryAnalyzer/Ptrs.h"

#include <vector>
using namespace std;

class AosStatQueryPattern1 : public OmnRCObject 
{
	OmnDefineRCObject;	

public:
	AosStatQueryPattern1();
	~AosStatQueryPattern1();

	bool 	proc(
				const AosRundataPtr &rdata,
				const AosQueryReqObjPtr &req,
				const AosStatDefineDocPtr &stat_def);

	bool 	generateRslt(
				const AosRundataPtr &rdata,
				const AosStatQueryUnitPtr &stat_unit,
				OmnString &content);
	
private:
	bool 	findMeasureFields(
				const AosRundataPtr &rdata,
				const AosQueryReqObjPtr &req,
				const AosStatDefineDocPtr &stat_def,
				set<AosStatMeasure> &measures);

	bool 	runQuery(
				const AosRundataPtr &rdata,
				const AosQueryReqObjPtr &req,
				const AosStatQueryUnitPtr &stat_unit);

	bool 	trimQueryReqTerms(
				const AosRundataPtr &rdata,
				const AosQueryTermObjPtr &or_term,
				const AosStatQueryUnitPtr &stat_unit);

	bool 	collectStatDocids(
				const AosRundataPtr &rdata,
				const AosQueryTermObjPtr &or_term,
				u32 &docid_num,
				AosBuffPtr &docids_buff);

};

#endif
