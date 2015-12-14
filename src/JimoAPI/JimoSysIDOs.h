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
// 2015/03/29 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_JimoAPI_JimoSysIDOs_h
#define Aos_JimoAPI_JimoSysIDOs_h

#include "SEInterfaces/Ptrs.h"
#include "Util/String.h"

class AosRundata;

namespace Jimo
{
	extern AosSeqnoIDOObjPtr jimoCreateSeqnoIDO(
									AosRundata *rdata, 
									const OmnString &name, 
									const bool override_flag);

	extern AosU64IDOObjPtr jimoCreateU64IDO(
									AosRundata *rdata, 
									const OmnString &name, 
									const bool override_flag);
};

#endif
