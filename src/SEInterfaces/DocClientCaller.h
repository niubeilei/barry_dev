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
// 2013/04/08 Created by Linda
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SEInterfaces_DocClientCaller_h
#define Aos_SEInterfaces_DocClientCaller_h

#include "alarm_c/alarm.h"
#include "Rundata/Ptrs.h"
#include "SEInterfaces/Ptrs.h"
#include "Util/String.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "XmlUtil/Ptrs.h"
#include "XmlUtil/XmlTag.h"

class AosDocClientCaller : virtual public OmnRCObject
{
public:
	virtual bool docClientCallback(
					const AosRundataPtr &rdata, 
					const OmnString &id,
					const AosBuffPtr &buff,
					const bool finished) = 0;
};
#endif
