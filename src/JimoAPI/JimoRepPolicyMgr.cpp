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
// 2015/03/15 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "JimoAPI/JimoRepPolicyMgr.h"

#include "UtilData/RepPolicy.h"

static u32 sgCubeMapRepPolicy = AosRepPolicy::getRepPolicy(3, 0, 0);
static u32 sgDocRepPolicy = AosRepPolicy::getRepPolicy(3, 0, 0);
static u32 sgIndexRepPolicy = AosRepPolicy::getRepPolicy(3, 0, 0);
static u32 sgNoRepPolicy = AosRepPolicy::getRepPolicy(1, 0, 0);

namespace Jimo
{

RepPolicyMgr gRepPolicyMgr;

u32
RepPolicyMgr::getCubeMapRepPolicy()
{
	return sgCubeMapRepPolicy;
}


u32
RepPolicyMgr::getDocRepPolicy()
{
	return sgDocRepPolicy;
}


u32
RepPolicyMgr::getIndexRepPolicy()
{
	return sgIndexRepPolicy;
}


};
