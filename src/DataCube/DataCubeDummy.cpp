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
// 2013/11/24 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "DataCube/DataCubeDummy.h"

#include "Rundata/Rundata.h"
#include "XmlUtil/XmlTag.h"



AosDataCubeDummy::AosDataCubeDummy(const u32 version)
:
AosDataCube(AOS_DATACUBETYPE_DUMMY, version)
{
}


AosDataCubeDummy::~AosDataCubeDummy()
{
}

