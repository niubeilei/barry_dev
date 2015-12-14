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
// 01/02/2011	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "SEInterfaces/DocEngineObj.h"

#include "Rundata/Rundata.h"



AosDocEngineObjPtr AosDocEngineObj::smDocEngine;


bool 
AosDocEngineObj::removeDocByDocidStatic(
		const AosRundataPtr &rdata, 
		const u64 docid)
{
	aos_assert_rr(smDocEngine, rdata, false);
	return smDocEngine->deleteDocByDocid(rdata, docid);
}


