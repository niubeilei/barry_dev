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
// 2015/03/29 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SEInterfaces_U64IDO_h
#define Aos_SEInterfaces_U64IDO_h

#include "IDO/IDO.h"
#include "SEInterfaces/Ptrs.h"


class AosRundata;

class AosU64IDOObj : virtual public AosIDO
{
public:
	enum
	{
		eMethodCreateByName,
		eMethodGet,
		eMethodSet,
	};

public:
	AosU64IDOObj(const int version);

	virtual u64 get(AosRundata *rdata, const u64 dft) = 0;
	virtual bool set(AosRundata *rdata, const u64 value) = 0;
};
#endif
