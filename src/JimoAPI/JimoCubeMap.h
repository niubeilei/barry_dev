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
//
// Modification History:
// 2015/03/20 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_JimoAPI_JimoCubeMap_h
#define Aos_JimoAPI_JimoCubeMap_h

//#include "JimoObj/JimoCubeMapObj.h"
//#include "JimoObj/Ptrs.h"
#include "SEInterfaces/CubeMapObj.h"

class AosRundata;

namespace Jimo
{
	extern AosCubeMapObjPtr jimoCreateCubeMap(AosRundata *rdata, const AosXmlTagPtr &conf);
	extern AosCubeMapObjPtr jimoCreateCubeMap(AosRundata *rdata, AosBuff *buff);

	extern bool jimoGetTargetCubes(AosRundata *rdata, 
			               	const u32 cube_id, 
							vector<AosCubeMapObj::CubeInfo> &cubes, 
							const u32 replic_policy);

	extern bool jimoGetTargetCubes(AosRundata *rdata, 
			                vector<AosCubeMapObj::CubeInfo> &cubes);
};

#endif
