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
// 01/17/2014	Created by Young
////////////////////////////////////////////////////////////////////////////
#include "JQLStatement/JqlStmtDebug.h"

#include "Debug/Debug.h"
#include "JQLExpr/Expr.h"
#include "Rundata/Rundata.h"
#include "Util/OmnNew.h"
#include "XmlUtil/XmlTag.h"
#include "API/AosApi.h"
#include "SEInterfaces/ActionObj.h"
#include "SEUtil/IILName.h"

AosJqlStmtDebug::AosJqlStmtDebug()
{
}


AosJqlStmtDebug::~AosJqlStmtDebug()
{
}


bool
AosJqlStmtDebug::run(const AosRundataPtr &rdata, const AosJimoProgObjPtr &prog)
{
	if (mOption == "")
	{
		AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		OmnAlarm << __func__ << enderr;
		return rdata;
	}
	if (mOption == "on")
	{
		// Print on
		OmnTracer::mStdio = true;
		OmnCout << "Debug message turn on" << endl;
	}
	else if (mOption == "off")
	{
		// Print off
		OmnTracer::mStdio = false;
		OmnCout << "Debug message turn off" << endl;
	}

	return true;
}



AosJqlStatement *
AosJqlStmtDebug::clone()
{
	return OmnNew AosJqlStmtDebug(*this);
}


void 
AosJqlStmtDebug::dump()
{
}


/***************************
 * Getter/setter
 * **************************/
void 
AosJqlStmtDebug::setSwitch(OmnString opt)
{
	aos_assert(opt != "");
	mOption = opt.toLower();
}


