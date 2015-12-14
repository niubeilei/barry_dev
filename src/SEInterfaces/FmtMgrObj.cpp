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
// 2013/07/16 Created by Ketty 
////////////////////////////////////////////////////////////////////////////
#include "SEInterfaces/FmtMgrObj.h"

#include "API/AosApiI.h"
#include "SEInterfaces/CubeTransSvrObj.h"

//AosFmtMgrObjPtr AosFmtMgrObj::smFmtMgr = 0;
AosFmtMgrObjPtr
AosFmtMgrObj::getFmtMgr()
{
	if(!AosIsSelfCubeSvr())	return 0;

	AosCubeTransSvrObjPtr trans_svr = AosCubeTransSvrObj::getTransSvr();
	aos_assert_r(trans_svr, 0);
	
	return trans_svr->getFmtSvr();	
}

