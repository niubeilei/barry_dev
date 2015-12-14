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
#ifndef AOS_Patterns_Pattern_h
#define AOS_Patterns_Pattern_h

#include "Pattern/Ptrs.h"
#include "SEInterfaces/PatternObj.h"
#include "Thread/Ptrs.h"
#include <vector>

class AosPattern : public AosPatternObj
{
protected:

public:
	AosPattern(const int version);
	AosPattern();

	~AosPattern();

};
#endif

