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
#include "JQLStatement/JqlHaving.h"

#include "Debug/Debug.h"
#include "Rundata/Rundata.h"
#include "Util/OmnNew.h"
#include "XmlUtil/XmlTag.h"
#include "API/AosApi.h"
#include "XmlUtil/Ptrs.h"

	
AosJqlHaving::AosJqlHaving()
:
mHavingConf(0)
{
}

AosJqlHaving::~AosJqlHaving()
{
}


void
AosJqlHaving::setHavingExpr(AosExprObj* expr)
{
	mHavingConf = expr->dumpByStat();
}

bool
AosJqlHaving::setHavingConf(const OmnString conf)
{
	mHavingConf = conf;
	return true;
}


OmnString 
AosJqlHaving::getHavingConf()
{
	return mHavingConf;
}
