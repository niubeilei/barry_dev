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
#ifndef Aos_SemanticData_VarAtomic_h
#define Aos_SemanticData_VarAtomic_h

#include "RVG/Ptrs.h"
#include "SemanticData/Var.h"
#include "SemanticData/Ptrs.h"


class AosVarAtomic : public AosVar
{
private:
	AosValuePtr		mValue;

public:
	AosVarAtomic(const std::string &name);
	~AosVarAtomic();

	virtual AosValuePtr	getValue() const;
};
#endif

