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
#include "JQLStatement/JqlOrderBy.h"
#include "JQLStatement/JqlStmtTable.h"

#include "SEInterfaces/QueryType.h" 
#include "JQLExpr/ExprMemberOpt.h"

#include "Debug/Debug.h"
#include "Rundata/Rundata.h"
#include "Util/OmnNew.h"
#include "XmlUtil/XmlTag.h"
#include "API/AosApi.h"
#include "XmlUtil/Ptrs.h"


AosJqlOrderBy::AosJqlOrderBy()
{
	mOrderFieldList = 0;
}


AosJqlOrderBy::~AosJqlOrderBy()
{
	OmnDelete mOrderFieldList;
	mOrderFieldList = 0;
}

void 
AosJqlOrderBy::setOrderFieldList(AosJqlOrderByFieldList *field_list)
{
	mOrderFieldList = field_list;
}

AosJqlOrderByFieldList*
AosJqlOrderBy::getOrderFieldList()
{	
	return  mOrderFieldList;
}

