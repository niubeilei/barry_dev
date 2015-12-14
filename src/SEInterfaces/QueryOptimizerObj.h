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
// 2013/12/29 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SEInterfaces_QueryOptimizerObj_h
#define Aos_SEInterfaces_QueryOptimizerObj_h

#include "Jimo/Jimo.h"
#include "Rundata/Ptrs.h"
#include "SEInterfaces/DatasetId.h"
#include "SEInterfaces/Ptrs.h"


class AosQueryOptimizerObj : virtual public AosJimo
{
public:
	AosQueryOptimizerObj(const u32 version);
	~AosQueryOptimizerObj();

	static AosQueryOptimizerObjPtr createQueryOptimizerStatic(
					AosRundata *rdata, 
					const AosXmlTagPtr &def);
};
#endif

