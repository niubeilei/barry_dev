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
#ifndef Aos_DataCube_DataCubeDummy_h
#define Aos_DataCube_DataCubeDummy_h

#include "DataCube/DataCube.h"


class AosDataCubeDummy : public AosDataCube
{

public:
	AosDataCubeDummy(const u32 version);
	~AosDataCubeDummy();
};
#endif



