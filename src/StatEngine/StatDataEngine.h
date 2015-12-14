///////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet DataEngineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet DataEngineering, Inc. or its derivatives
//
// Description:
//   
//
// Modification History:
// 2014/09/30 Created by Phil 
// 
////////////////////////////////////////////////////////////////////////////

#ifndef StatEngine_StatDataEngine_h
#define StatEngine_StatDataEngine_h

#include "StatServer/Ptrs.h"
#include "StatServer/StatKeyConn.h"
#include "StatUtil/StatModifyInfo.h"
#include "StatUtil/StatQueryInfo.h"
#include "StatUtil/StatRecord.h"
#include "Vector2DQryRslt/Ptrs.h"
#include "Vector2DUtil/Vt2dInfo.h"
#include "Vector2DUtil/Vt2dRecord.h"
#include "Vector2DUtil/Vt2dModifyInfo.h"
#include "Vector2D/Ptrs.h"

#include <vector>
#include <map>
using namespace std;

class AosStatDataEngine: public OmnRCObject
{

private:
	///////////////////////////////////////////////////////
	//         Basic members
	///////////////////////////////////////////////////////
	AosRundata* 				mRundata;

	//put StatModify info members here

	///////////////////////////////////////////////////////
	//         different counters
	///////////////////////////////////////////////////////
	u64			mModifyVt2dTime;
	u64			mModifyVt2dNum;

public:
	///////////////////////////////////////////////////////
	//       Constructors/Destructors
	///////////////////////////////////////////////////////
	AosStatDataEngine(AosRundata* rdata); 
	~AosStatDataEngine();

	///////////////////////////////////////////////////////
	//        basic methods
	///////////////////////////////////////////////////////
	bool init();
	
	bool runData();

	bool 	serializeTo(
					const AosRundataPtr &rdata,
					AosBuff *buff);

	bool 	serializeFrom(
					const AosRundataPtr &rdata,
					AosBuff * buff);

	///////////////////////////////////////////////////////
	//   Helper methods
	///////////////////////////////////////////////////////

	///////////////////////////////////////////////////////
	//        Counters methods
	///////////////////////////////////////////////////////
	void initCounters();
	void outputCounters();

};

#endif
