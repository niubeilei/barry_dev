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
#ifndef Aos_SEInterfaces_SeqnoIDO_h
#define Aos_SEInterfaces_SeqnoIDO_h

#include "IDO/IDO.h"
#include "SEInterfaces/Ptrs.h"


class AosRundata;

class AosSeqnoIDOObj : virtual public AosIDO
{
public:
	enum
	{
		eMethodNextSeqno,
		eMethodCreateByName,
	};

public:
	AosSeqnoIDOObj(const int version);

	virtual u64 nextSeqno(AosRundata *rdata) = 0;
};
#endif
