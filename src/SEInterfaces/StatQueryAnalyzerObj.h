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
// 2013/01/13 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SEInterfaces_StatQueryAnalyzer_h
#define Aos_SEInterfaces_StatQueryAnalyzer_h

#include "Rundata/Ptrs.h"
#include "SEInterfaces/Ptrs.h"
#include "Util/String.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "XmlUtil/Ptrs.h"
#include <vector>
using namespace std;

class AosStatQueryAnalyzerObj : virtual public OmnRCObject
{
private:
	static AosStatQueryAnalyzerObjPtr smAnalyzer;

public:
	static bool runQueryStatic(
					const AosRundataPtr &rdata,
					const AosQueryReqObjPtr &query);
	
	static bool generateRsltStatic(
					const AosRundataPtr &rdata,
					const AosQueryReqObjPtr &query,
					OmnString &contents);
	
	virtual bool analyzeReq(
					const AosRundataPtr &rdata,
					const AosQueryReqObjPtr &req) = 0;
	
	virtual bool generateRslt(
					const AosRundataPtr &rdata,
					const AosQueryReqObjPtr &query,
					OmnString &contents) = 0;
	
	static AosStatQueryAnalyzerObjPtr getObject() {return smAnalyzer;}
	static void setObject(const AosStatQueryAnalyzerObjPtr &d) {smAnalyzer= d;}
};
#endif
