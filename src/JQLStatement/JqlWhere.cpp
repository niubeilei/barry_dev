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
// 2014/05/07 Created By Andy Zhang 
////////////////////////////////////////////////////////////////////////////
#include "JQLStatement/JqlWhere.h"

#include "Rundata/Rundata.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/Ptrs.h"
#include "Util/OmnNew.h"
#include "Debug/Debug.h"
#include "API/AosApi.h"


AosJqlWhere::AosJqlWhere()
{
}

AosJqlWhere::~AosJqlWhere()
{
}

void 
AosJqlWhere::setWhereExpr(AosExprObj* expr)
{
	mWhereExpr = expr;
}

AosExprObjPtr
AosJqlWhere::getWhereExpr()
{
	return mWhereExpr;
}

