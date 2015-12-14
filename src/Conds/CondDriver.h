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
// 02/26/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_Conds_CondDriver_h
#define AOS_Conds_CondDriver_h

#include "Rundata/Ptrs.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"
#include "XmlUtil/Ptrs.h"


class AosCondDriver : public OmnRCObject
{
	OmnDefineRCObject;

public:
	AosCondDriver() {}
	~AosCondDriver() {};

	// AosConditionObj Interface
	virtual bool evalCond(const AosXmlTagPtr &def, const AosRundataPtr &rdata);
	virtual bool evalCond(const AosXmlTagPtr &def, 
						  const OmnString &name, 
						  const AosRundataPtr &rdata);
};
#endif

