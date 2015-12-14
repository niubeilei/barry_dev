////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
//
// Modification History:
// 04/28/2013 Created by Linda Lin 
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SEInterfaces_ActionCaller_h
#define Aos_SEInterfaces_ActionCaller_h

#include "Rundata/Ptrs.h"
#include "Util/Buff.h"
#include "Util/MemoryChecker.h"
#include "Util/String.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"
#include "Util/Ptrs.h"
#include "XmlUtil/Ptrs.h"

class AosActionCaller : virtual public OmnRCObject
{
public:
	virtual void callBack(const u64 &reqId, const int64_t &expected_size, const bool &finished) = 0;
};
#endif

