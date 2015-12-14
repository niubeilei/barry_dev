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
// 03/08/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_CloudCube_CubeMap_h
#define Aos_CloudCube_CubeMap_h

#include "Util/RCObject.h"
#include "Util/RCObjImp.h"

class AosCubeMap : public OmnRCObject
{
	OmnDefineRCObject;

public:
	AosCubeMap(const u64 &cubic_id);
	~AosCubeMap();

	bool config(const AosXmlTagPtr &conf);
	bool start();
	bool stop();

	virtual int cubeid2PhysicalId(const u32 cubeid);
};
#endif

