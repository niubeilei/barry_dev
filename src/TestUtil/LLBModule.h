////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: LLBModule.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_TestUtil_LLBModule_h
#define Omn_TestUtil_LLBModule_h

#include "Util/String.h"

class OmnLLBModule
{
public:
	static int		sample(const OmnString &agentName, const int value1, int *value2);
};


#endif

