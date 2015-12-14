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
// 2014/07/27 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_LogParser_LogParserMulti_h
#define AOS_LogParser_LogParserMulti_h

#include "LogParserMulti/Ptrs.h"
#include "SEInterfaces/LogParserMultiObj.h"
#include "Thread/Ptrs.h"
#include <vector>

class AosLogParserMulti : public AosLogParserMultiObj
{
protected:

public:
	AosLogParserMulti(const int version);

	~AosLogParserMulti();

	// LogParserMulti Interface
};
#endif

