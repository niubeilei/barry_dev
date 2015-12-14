////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//      
// Description: just for test
//
// Modification History:
// 2015/04/07 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SEInterfaces_ExprBracketObj_h
#define Aos_SEInterfaces_ExprBracketObj_h

#include "SEInterfaces/Ptrs.h"
#include "SEInterfaces/ExprObj.h"

class AosExprBracketsObj 
{
public:
	// virtual vector<AosExprNameValueObjPtr> getExprList(AosRundata *rdata) = 0;
	virtual vector<AosExprObjPtr> getExprList(AosRundata *rdata) = 0;
};
#endif

