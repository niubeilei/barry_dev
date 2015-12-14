////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//	
// Description:
// This is a super class for data cubes that contain multiple files.
//
// Modification History:
// 2013/12/23 Created by Chen Ding
// 2014/01/21 Modifyed by Ketty from DataCubeFileGroup 
//
////////////////////////////////////////////////////////////////////////////
#include "DataCube/Jimos/DataCubeCtnr.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "API/AosApi.h"
#include "API/AosApiG.h"
#include "Rundata/Rundata.h"
#include "XmlUtil/Ptrs.h"


AosDataCubeCtnr::AosDataCubeCtnr(
		const OmnString &type,
		const u32 version)
:
AosDataCube(type, version),
mNextDataCubeIdx(0)
{
}


AosDataCubeCtnr::~AosDataCubeCtnr()
{
}


void
AosDataCubeCtnr::addDataConnector(const AosDataConnectorObjPtr &data_cube)
{
	mDataCubes.push_back(data_cube);
}


AosDataConnectorObjPtr
AosDataCubeCtnr::nextDataConnector()
{
	if(mNextDataCubeIdx == mDataCubes.size())	return 0;
	return mDataCubes[mNextDataCubeIdx++];
}


bool
AosDataCubeCtnr::readData(const u64 reqid, AosRundata *rdata)
{
	// just signal data_scanner will call this func.
	if(!mCrtDataCube)
	{
		mCrtDataCube = getSelfProcNextDataCube();
		if (!mCrtDataCube)
		{
			if (mDataCubes.empty())
			{
				mCaller->callBack(reqid, NULL, true);
				return true;
			}
			return true;
		}
	}

	mCrtDataCube->readData(reqid, rdata);
	return true;
}


void
AosDataCubeCtnr::callBack(
		const u64 &reqid,
		const AosBuffDataPtr &buff,
		const bool finished)
{
	// just signal data_scanner will call this func.
	bool all_finished = false;
	if(finished)
	{
		mCrtDataCube = getSelfProcNextDataCube();
		if (!mCrtDataCube) all_finished = true;
	}
	mCaller->callBack(reqid, buff, all_finished);
}


AosDataConnectorObjPtr
AosDataCubeCtnr::getSelfProcNextDataCube()
{
	AosDataConnectorObjPtr cube_stat = nextDataConnector();
	if (!cube_stat) return 0;
	
	AosDataConnectorCallerObjPtr thisptr(this, false);
	cube_stat->setCaller(thisptr);
	return cube_stat;	
}

void
AosDataCubeCtnr::setCaller(const AosDataConnectorCallerObjPtr &caller)
{
	// just signal data_scanner will call this func.
	mCaller = caller;
}


