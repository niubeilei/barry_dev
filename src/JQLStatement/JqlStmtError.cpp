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
// 2014/12/02	Created by Levi Wang
////////////////////////////////////////////////////////////////////////////
#include "JQLStatement/JqlStmtError.h"

#include "Debug/Error.h"
#include "JQLExpr/Expr.h"
#include "Rundata/Rundata.h"
#include "Util/OmnNew.h"
#include "XmlUtil/XmlTag.h"
#include "API/AosApi.h"
#include "SEInterfaces/ActionObj.h"
#include "SEUtil/IILName.h"

AosJqlStmtError::AosJqlStmtError(const OmnString &errmsg)
:
mErrmsg(errmsg)
{
}


AosJqlStmtError::~AosJqlStmtError()
{
}


bool
AosJqlStmtError::run(const AosRundataPtr &rdata, const AosJimoProgObjPtr &prog)
{
	AosSetErrorUser(rdata,  __func__) << mErrmsg << enderr;
	OmnAlarm << __func__ << enderr;
	return false;
}


AosJqlStatement *
AosJqlStmtError::clone()
{
	return OmnNew AosJqlStmtError(*this);
}


void 
AosJqlStmtError::dump()
{
}

