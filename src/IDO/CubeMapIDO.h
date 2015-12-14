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
#ifndef Aos_IDO_CubeMapIDO_h
#define Aos_IDO_CubeMapIDO_h

#include "IDO/IDO.h"


class AosCubeMapIDO : public AosIDO
{
public:
	virtual bool getCubeAllocation(AosRundata *rdata, vector<int> &endpoint_ids) const = 0;
	virtual bool modifyCubeAllocation(AosRundata *rdata, const vector<int> &endpoint_ids) = 0;
};
#endif
