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
#ifndef Aos_SEInterfaces_QueryEngineObj_h
#define Aos_SEInterfaces_QueryEngineObj_h

#include "Jimo/Jimo.h"
#include "Rundata/Ptrs.h"
#include "SEInterfaces/Ptrs.h"
#include "Util/String.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "XmlUtil/Ptrs.h"
#include <vector>
using namespace std;

class AosQueryEngineObj : public AosJimo
{
private:
	static AosQueryEngineObjPtr smObject;

public:
	AosQueryEngineObj(const int version);

	virtual bool runQuery(
					const AosRundataPtr &rdata,
					const AosQueryReqObjPtr &query, 
					const AosQueryProcCallbackPtr &callback) = 0;
	virtual bool sendMsg(	
					const int cube_id, 
					const OmnString &msg, 
					const AosRundataPtr &rdata){return false;}

	static bool hasSectionsOnCube(
					const AosRundataPtr &rdata, 
					const int cube_id, 
					const vector<u64> &queried_sections, 
					vector<u64> &expected_sections);

	static bool runQueryStatic(
					const AosRundataPtr &rdata, 
					const AosQueryReqObjPtr &query, 
					const AosQueryProcCallbackPtr &callback);
	static AosQueryEngineObjPtr getQueryEngine();
};
#endif
