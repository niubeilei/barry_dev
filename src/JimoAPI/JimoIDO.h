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
// 2015/03/18 Created by Gavin
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_JimoAPI_JimoIDOMgr_h
#define Aos_JimoAPI_JimoIDOMgr_h

#include "SEInterfaces/Ptrs.h"
#include "Util/String.h"
#include "Util/Ptrs.h"
#include "IDO/Ptrs.h"

class AosRundata;

namespace Jimo
{
	extern AosIDOPtr jimoGetIDO(AosRundata *rdata, const u64 docid);
	extern AosBuffPtr jimoGetIDOBuff(AosRundata *rdata, const u64 docid);
	extern bool jimoCreateIDO( AosRundata *rdata, 
								const OmnString &obj_name, 
								AosIDO *ido,
								const bool override_flag);

	// The IDO example
	extern bool jimoIncCounter(AosRundata *rdata, 
								const OmnString &name,
								const i32 inc_value, 
								i64 &crt_value);
};
#endif

