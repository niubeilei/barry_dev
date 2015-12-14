////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// Modification History:
// 2011/06/01	Created by Ken Lee
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_Timer_TimerObj_h
#define AOS_Timer_TimerObj_h

#include "Util/RCObject.h"
#include "Util/RCObjImp.h"


class AosTimerObj : virtual public OmnRCObject
{
private:
	AosRundataPtr 	mTimerRdata;

public:
	virtual bool	timeout(const u64 &timerid, const OmnString &udata, const AosRundataPtr &rdata) = 0;
	void 			setTimerRdata(const AosRundataPtr &rdata) {mTimerRdata = rdata;}
	AosRundataPtr 	getRdata() const {return mTimerRdata;}
};

#endif
