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
// 2013/10/09 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "JQLStatement/JqlStmtCompleteIIL.h"

#include "Debug/Debug.h"
#include "Rundata/Rundata.h"
#include "Util/OmnNew.h"


AosJqlStmtCompleteIIL::AosJqlStmtCompleteIIL()
{
}


AosJqlStmtCompleteIIL::~AosJqlStmtCompleteIIL()
{
}


bool
AosJqlStmtCompleteIIL::run(const AosRundataPtr &rdata, const AosJimoProgObjPtr &prog)
{
	if (mIILName == "") 
	{
		rdata->setJqlMsg("ERROR: iilname is null");	
		return false;
	}

	if ( rebuildBitmap(rdata, mIILName) )
	{
		rdata->setJqlMsg("Complete IIL sucessful");
		return true;
	}

	OmnString msg = "ERROR: rebuild bitmap failed";
	rdata->setJqlMsg(msg);
	return false;
}


AosJqlStatement *
AosJqlStmtCompleteIIL::clone()
{
	return OmnNew AosJqlStmtCompleteIIL(*this);
}


void 
AosJqlStmtCompleteIIL::dump()
{
}


void 
AosJqlStmtCompleteIIL::setIILName(const OmnString &iilname)
{
	OmnString iilcompose = AOSZTG_SUPERIIL;
	if (iilname.length() >= iilcompose.length() && 
			strncmp(iilcompose.data(), iilname.data(), size_t(iilcompose.length())) == 0 )
	{
		mIILName = iilname;
	}
	else
	{
		mIILName = "";
		mIILName << iilcompose << "_" << iilname;
	}
}


