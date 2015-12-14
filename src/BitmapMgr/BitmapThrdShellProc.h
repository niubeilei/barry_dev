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
// based on the string value. 
//
// Modification History:
// 07/09/2012 Created by Linda 
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_BitmapMgr_BitmapThrdShellProc_h
#define AOS_BitmapMgr_BitmapThrdShellProc_h

#include "Rundata/Rundata.h"
#include "Thread/ThreadedObj.h"
#include "Thread/ThrdShellProc.h"


class AosBmpStartThrd : public OmnThrdShellProc
{
	OmnDefineRCObject;

	AosRundataPtr   			mRundata;

public:
	AosBmpStartThrd(
			const AosRundataPtr &rdata);
	~AosBmpStartThrd();

	virtual bool    run();
	virtual bool    procFinished(){return true;}
	virtual bool    waitUntilFinished(){return true;}
};
#endif

