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
// 07/21/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SEInterfaces_DataCollectorCreatorObj_h
#define Aos_SEInterfaces_DataCollectorCreatorObj_h

#include "Rundata/Ptrs.h"
#include "Rundata/Rundata.h"
#include "SEInterfaces/Ptrs.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"
#include "Util/Ptrs.h"
#include "XmlUtil/Ptrs.h"


class AosDataCollectorCreatorObj : virtual public OmnRCObject
{
	OmnDefineRCObject;

public:
	virtual AosDataCollectorObjPtr createDataCollector(
							const AosXmlTagPtr &def, 
							const AosRundataPtr &rdata) = 0;
	
	virtual bool registerDataCollector(
							const OmnString &name, 
							const AosDataCollectorObjPtr &cacher) = 0;

	virtual AosDataCollectorObjPtr serializeFrom(
							const AosBuffPtr &buff, 
							const AosRundataPtr &rdata) = 0;
	
	virtual bool checkConfig(
							const AosXmlTagPtr &def, 
							const AosRundataPtr &rdata) = 0;
};

#endif

