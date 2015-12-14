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
// 4/16/2007: Created by cding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_AmClient_AmApiProc_h
#define Aos_AmClient_AmApiProc_h

#include "Thread/Ptrs.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"


class AosAmApiProc : virtual public OmnRCObject
{
	OmnDefineRCObject;

protected:

public:
	AosAmApiProc();
	~AosAmApiProc();

	virtual bool	proc(const OmnThreadShellPtr &shell) = 0;

private:
};

#endif

