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
#ifndef AOS_JQLStatement_JqlHaving_H
#define AOS_JQLStatement_JqlHaving_H

#include "JQLStatement/JqlDataStruct.h"
#include "SEInterfaces/ExprObj.h"
#include "Util/String.h"

class AosJqlHaving : public AosJqlDataStruct 
{
public:
	AosExprObjPtr		mHavingExpr;
	OmnString 			mHavingConf;

public:
	AosJqlHaving();
	
	~AosJqlHaving();
	void setHavingExpr(AosExprObj* expr);
	AosExprObjPtr getHavingExpr();       
	bool setHavingConf(const OmnString conf);
	OmnString getHavingConf();

};

#endif
