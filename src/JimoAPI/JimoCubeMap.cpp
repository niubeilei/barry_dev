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
//
// Modification History:
// 2015/03/20 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "JimoAPI/JimoCubeMap.h"

//#include "JimoAPI/Jimo.h"
#include "SEInterfaces/CubeMapObj.h"
#include "Rundata/Rundata.h"
#include "Thread/Mutex.h"

static AosCubeMapObj *sgCubeMap = 0;
static OmnMutex sgJimoLock;

bool sgLoadCubeMap(AosRundata *rdata)
{
	sgJimoLock.lock();
	if (sgCubeMap)
	{
		sgJimoLock.unlock();
		return true;
	}

	AosJimoPtr jimo = AosCreateJimoByClassname(rdata, "AosCubeMap", 1);
	if (!jimo)
	{
		sgJimoLock.unlock();
		AosLogError(rdata, false, "internal_error") << enderr;
		return false;
	}

	sgCubeMap = dynamic_cast<AosCubeMapObj *>(jimo.getPtr());
	if (!sgCubeMap)
	{
		sgJimoLock.unlock();
		AosLogError(rdata, false, "internal_error") << enderr;
		return false;
	}
	sgJimoLock.unlock();
	return true;
}

namespace Jimo
{

AosCubeMapObjPtr jimoCreateCubeMap(AosRundata *rdata, const AosXmlTagPtr &conf) 
{
	if (!sgCubeMap) sgLoadCubeMap(rdata);
	aos_assert_rr(sgCubeMap, rdata, 0);
	return sgCubeMap->createCubeMap(rdata, conf);
}


AosCubeMapObjPtr jimoCreateCubeMap(AosRundata *rdata, AosBuff *buff) 
{
	if (!sgCubeMap) sgLoadCubeMap(rdata);
	aos_assert_rr(sgCubeMap, rdata, 0);
	return sgCubeMap->createCubeMap(rdata, buff);
}


bool jimoGetTargetCubes(
		AosRundata *rdata, 
		const u32 cube_id, 
		vector<AosCubeMapObj::CubeInfo> &cubes, 
		const u32 replic_policy, 
		const bool write_only_flag)
{
	if (!sgCubeMap) sgLoadCubeMap(rdata);
	aos_assert_rr(sgCubeMap, rdata, 0);
	return sgCubeMap->getTargetCubes(rdata, cube_id, cubes);
}


bool jimoGetTargetCubes(AosRundata *rdata, vector<AosCubeMapObj::CubeInfo> &cubes)
{
	if (!sgCubeMap) sgLoadCubeMap(rdata);
	aos_assert_rr(sgCubeMap, rdata, 0);
	return sgCubeMap->getTargetCubes(rdata, cubes);
}


};

