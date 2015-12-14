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
// The super class for all Atomic type semantics data. 
//
// Modification History:
// 11/27/2007: Created by Chen Ding
// 
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SemanticData_ExeEnv_h
#define Aos_SemanticData_ExeEnv_h

#include "SemanticData/SemanticData.h"
#include "SemanticData/Ptrs.h"


class AosExeEnv : public AosSemanticData
{
private:

public:
	AosExeEnv();
	~AosExeEnv();
};
#endif

