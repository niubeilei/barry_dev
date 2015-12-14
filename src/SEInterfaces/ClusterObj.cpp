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
// 2015/03/13 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "SEInterfaces/ClusterObj.h"

#include "Util/UtUtil.h"


AosClusterObj::AosClusterObj()
{
}


u32 
AosClusterObj::getDistrID(AosRundata *rdata, const OmnString &name)
{
	aos_assert_rr(getNumCubes() > 0, rdata, 0);
	u64 id = AosStrHashFunc(name.data(), name.length());
	return id % getNumCubes();
}


u32 
AosClusterObj::getDistrID(AosRundata *rdata, const u64 docid)
{
	aos_assert_rr(getNumCubes() > 0, rdata, 0);
	return docid % getNumCubes();
}


