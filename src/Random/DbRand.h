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
// 01/13/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_RANDOM_DbRand_h
#define AOS_RANDOM_DbRand_h

#include "Rundata/Ptrs.h"
#include "Util/String.h"


class AosDbRand 
{
public:
	static OmnString pickUserDomain(AosRundata *rdata);
	static OmnString pickContainer(AosRundata *rdata);
};
#endif

