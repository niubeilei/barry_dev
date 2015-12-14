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
// 2014/11/16 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_SEInterfaces_GenericValueObj_H
#define AOS_SEInterfaces_GenericValueObj_H


#include "SEInterfaces/ExprObj.h"
#include "SEInterfaces/Ptrs.h"
#include "Util/String.h"
#include <vector>
using namespace std;

class AosGenericValueObj : virtual public OmnRCObject
{
	OmnDefineRCObject;

public:
	AosExprObjPtr			mName;
	AosExprObjPtr			mValue;
	vector<AosExprObjPtr>   mArrayValues;

public:
	AosGenericValueObj(){}
	~AosGenericValueObj(){}

	OmnString toString() const {return "";}
	AosExprObjPtr getValue() const {return mValue;}
};

#endif
