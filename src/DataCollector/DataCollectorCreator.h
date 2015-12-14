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
// 07/19/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_DataCollector_DataCollectorCreator_h
#define Aos_DataCollector_DataCollectorCreator_h

#include "DataCollector/DataCollector.h"
#include "SEInterfaces/DataCollectorCreatorObj.h"

#if 0
class AosDataCollectorCreator : virtual public AosDataCollectorCreatorObj
{
	OmnDefineRCObject;

public:
	virtual bool registerDataCollector(
							const OmnString &name, 
							const AosDataCollectorObjPtr &collector);

	virtual AosDataCollectorPtr createDataCollector(
							const AosXmlTagPtr &def, 
							const AosRundataPtr &rdata);

	virtual AosDataCollectorPtr serializeFrom(
							const AosBuffPtr &buff, 
							const AosRundataPtr &rdata);
	
	virtual bool checkConfig(
							const AosXmlTagPtr &def, 
							const AosRundataPtr &rdata);
};
#endif
#endif

