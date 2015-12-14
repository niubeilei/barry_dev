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
// 2013/09/16 Created by Jackie 
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SEInterfaces_QueryReqCaller_h
#define Aos_SEInterfaces_QueryReqCaller_h

#include "alarm_c/alarm.h"
#include "Rundata/Ptrs.h"
#include "SEInterfaces/Ptrs.h"
#include "Util/String.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "XmlUtil/Ptrs.h"
#include "XmlUtil/XmlTag.h"

class AosQueryReqCaller : virtual public OmnRCObject
{

public:
	virtual bool fetchFieldsFinished(
		const AosRundataPtr &rdata, 
		const AosBuffPtr &buff) = 0;
};
#endif
