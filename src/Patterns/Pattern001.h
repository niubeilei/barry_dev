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
// 2014/08/03 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_Patterns_Pattern001_h
#define AOS_Patterns_Pattern001_h

#include "Pattern/Ptrs.h"
#include "SEInterfaces/PatternObj.h"
#include "Thread/Ptrs.h"
#include <vector>

class AosPattern001 : public AosPattern
{
protected:

public:
	AosPattern001(const int version);
	AosPattern001();

	~AosPattern();
};
#endif

