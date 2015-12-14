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
//
// Modification History:
// 2011/01/22	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Omn_SeReqProc_SeReqUtil_h
#define Omn_SeReqProc_SeReqUtil_h

#include "SEServer/Ptrs.h"
#include "SEServer/SeReqProc.h"

inline AosSeReqProcPtr AosGetSeReqProc(const AosRundataPtr &rdata)
{
	AosNetReqProcPtr reqproc = rdata->getReqProc();
	if (!reqproc) return 0;

	AosSeReqProcPtr rr = (AosSeReqProc*)reqproc.getPtr();
	return rr;
}

#endif

