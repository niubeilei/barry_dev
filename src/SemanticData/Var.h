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
// The super class for all semantics data. 
//
// Modification History:
// 11/26/2007: Created by Chen Ding
// 01/28/2008: Changed to Variable
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SemanticData_Var_h
#define Aos_SemanticData_Var_h

#include "aosUtil/Types.h"
#include "RVG/Ptrs.h"
#include "SemanticData/Types.h"
#include "Util/RCObject.h"

#include <string>


class AosVar : virtual public OmnRCObject
{
protected:
	std::string		mName;

public:
	AosVar(const std::string &name);
	~AosVar();

	virtual AosVarType::E	getType() const = 0;
	virtual AosValuePtr		getValue() const;
	std::string				getName() const {return mName;}
};
#endif

