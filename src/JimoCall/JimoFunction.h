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
// 2014/11/24 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_JimoCall_JimoFunc_h
#define Aos_JimoCall_JimoFunc_h

#include "Jimo/Jimo.h"

class AosJimoFunction : public AosJimo
{
public:
	AosJimoFunction(const int version);

	virtual bool callFunc(AosRundata *rdata, 
						const u32 from_epid, 
						const u32 to_epid, 
						const u64 jimocall_id, 
						AosBuff *buff) = 0;
};
#endif
