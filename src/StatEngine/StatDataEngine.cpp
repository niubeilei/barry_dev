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
// 2014/09/30 Created by Phil 
////////////////////////////////////////////////////////////////////////////

#include "StatEngine/StatDataEngine.h"

#include "Thread/Sem.h"
#include "SEInterfaces/RlbFileType.h"
#include "StatServer/Vt2dModifyReq.h"
#include "StatServer/Vt2dReadReq.h"
#include "StatUtil/StatTimeUnit.h"
#include "Thread/ThreadPool.h"
#include "Util/StrSplit.h"
#include "Vector2DQryRslt/Vt2dQryRslt.h"
#include "Vector2DQryRslt/Vt2dQryRsltProc.h"
#include "Vector2D/Vector2D.h"
#include "CounterUtil/CounterUtil.h"  
#include "Debug/Debug.h"

///////////////////////////////////////////////////////
//    Constructors/Destructors
///////////////////////////////////////////////////////
AosStatDataEngine::AosStatDataEngine(AosRundata* rdata) 
:
mRundata(rdata)
{
}

AosStatDataEngine::~AosStatDataEngine()
{
	//need to release memory
}

///////////////////////////////////////////////////////
//    basic methods
///////////////////////////////////////////////////////
bool
AosStatDataEngine::init()
{
	initCounters();
	return true;
}


//
//this methods save data into jimodb
//
bool 
AosStatDataEngine::runData()
{
	//at the end, update counters
	outputCounters();
	return true;
}

bool
AosStatDataEngine::serializeTo(
		const AosRundataPtr &rdata,
		AosBuff *buff)
{
	return true;
}


bool
AosStatDataEngine::serializeFrom(
		const AosRundataPtr &rdata,
		AosBuff *buff)
{
	return true;
}


///////////////////////////////////////////////////////
//   Helper methods used by both cube and frontend
///////////////////////////////////////////////////////


///////////////////////////////////////////////////////
//    Counters
///////////////////////////////////////////////////////
void
AosStatDataEngine::initCounters()
{
	//init different counters
	mModifyVt2dTime = 0;
	mModifyVt2dNum = 0;
}

void
AosStatDataEngine::outputCounters()
{
	OmnScreen << "(Statistics counters : Data Engine) Modify Vt2d values --- Time : "
		<< mModifyVt2dTime << ", Num: " << mModifyVt2dNum << endl;
}
