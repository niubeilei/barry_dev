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

#ifndef StatQueryAnalyzer_StatQueryAnalyzer_h
#define StatQueryAnalyzer_StatQueryAnalyzer_h

#include "Rundata/Ptrs.h"
#include "SEInterfaces/StatQueryAnalyzerObj.h"
#include "SEInterfaces/Ptrs.h"
#include "StatUtil/Ptrs.h"
#include "StatQueryAnalyzer/Ptrs.h"
#include "Util/String.h"
#include "XmlUtil/Ptrs.h"
#include "QueryUtil/QrUtil.h"

#include <vector>
using namespace std;

class AosStatQueryAnalyzer : public AosStatQueryAnalyzerObj
{
	OmnDefineRCObject;

private:
	AosStatQueryPattern1Ptr		mPattern1;

public:
	AosStatQueryAnalyzer();
	~AosStatQueryAnalyzer();

	virtual bool analyzeReq(
			const AosRundataPtr &rdata,
			const AosQueryReqObjPtr &req);
	
	virtual bool generateRslt(
				const AosRundataPtr &rdata,
				const AosQueryReqObjPtr &req,
				OmnString &content);

private:
	AosStatDefineDocPtr getStatDefDoc(
				const AosRundataPtr &rdata,
				const AosQueryReqObjPtr &req);

};

#endif
