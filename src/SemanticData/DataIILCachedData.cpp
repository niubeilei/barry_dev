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
// 2013/03/20 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "SemanticData/DataIILCachedData.h"

static int sgMap[AosDict::eMaxEntry];

AosDataIILCachedData::AosDataIILCachedData()
{
	mMap = smMap;
}


