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
#ifndef AOS_JQLStatement_JqlWhere_H
#define AOS_JQLStatement_JqlWhere_H

#include "JQLStatement/JqlStatement.h"
#include "Util/String.h"

struct AosJqlWhere : public AosJqlDataStruct
{
public:
	AosExprObjPtr	mWhereExpr;

public:
	AosJqlWhere();
	~AosJqlWhere();
	void setWhereExpr(AosExprObj* expr);
	AosExprObjPtr getWhereExpr();

};

#endif
