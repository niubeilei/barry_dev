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
// 11/01/2011	Created by Linda Lin 
////////////////////////////////////////////////////////////////////////////
#include "DataGenUtil/AllDataGenUtil.h"

#include "DataGenUtil/DataGenEnumRand.h"
#include "DataGenUtil/DataGenUtilType.h"
#include "DataGenUtil/DataGenSectionRand.h"
#include "DataGenUtil/DataGenSimpleRand.h"
#include "DataGenUtil/DataGenStringRand.h"
#include "DataGenUtil/DataGenComposeRand.h"
#include "DataGenUtil/DataGenSeqnoRand.h"

AosDataGenUtilPtr    sgDataGenUtil[AosDataGenUtil::eMax+1];
AosStr2U32_t     	 AosDataGenUtilType::smNameMap1;

AosAllDataGenUtil	gAosAllDataGenUtil;

AosAllDataGenUtil::AosAllDataGenUtil()
{
	static AosDataGenEnumRand 		sgDataGenEnumRand(true);
	static AosDataGenSectionRand	sgDataGenSectionRand(true);
	static AosDataGenSimpleRand		sgDataGenSimpleRand(true);
	static AosDataGenStringRand		sgDataGenStringRand(true);
	static AosDataGenComposeRand	sgDataGenComposeRand(true);
	static AosDataGenSeqnoRand		sgDataGenSeqnoRand(true);
}


