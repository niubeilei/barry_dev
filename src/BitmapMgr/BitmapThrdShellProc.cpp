////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// This type of IIL maintains a list of (string, docid) and is sorted
// based on the string value. It has a companion IIL that keeps the
// same set of Docids but sorted based on docids. 
//
// Modification History:
// 	Created: 03/30/2013 by Shawn 
////////////////////////////////////////////////////////////////////////////
#include "BitmapMgr/BitmapThrdShellProc.h"

#include "BitmapMgr/BitmapMgr.h"
#include "Porting/Sleep.h"


//========================== AosIILBigStrSaver =============================
AosBmpStartThrd::AosBmpStartThrd(
		const AosRundataPtr &rdata)
:
OmnThrdShellProc("BitmapStartThrd"),
mRundata(rdata->clone(AosMemoryCheckerArgsBegin))
{
}

AosBmpStartThrd::~AosBmpStartThrd()
{
}

bool		
AosBmpStartThrd::run()
{
	// save the iil
	OmnSleep(2);
	AosBitmapMgr::getSelf()->getIDGen(mRundata);
	return true;
}

