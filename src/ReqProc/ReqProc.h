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
#ifndef Omn_ReqProc_ReqProc_h
#define Omn_ReqProc_ReqProc_h

#include "Rundata/Ptrs.h"
#include "SEUtil/Ptrs.h"
#include "Util/RCObject.h"
#include "Util/String.h"
#include "XmlInterface/Ptrs.h"

class AosReqProc : virtual public OmnRCObject
{
public:
	AosReqProc() { }
	virtual ~AosReqProc() {}
	virtual bool proc(const AosRundataPtr &rdata) = 0;
};
#endif

