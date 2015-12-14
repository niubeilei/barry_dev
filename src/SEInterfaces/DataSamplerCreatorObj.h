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
// 07/23/2012 Created by Felicia
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SEInterfaces_DataSamplerCreatorObj_h
#define Aos_SEInterfaces_DataSamplerCreatorObj_h

#include "Rundata/Ptrs.h"
#include "SEInterfaces/Ptrs.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"
#include "Util/Ptrs.h"
#include "XmlUtil/Ptrs.h"


class AosDataSamplerCreatorObj : virtual public OmnRCObject
{
public:
	virtual AosDataSamplerObjPtr createStrSampler(const AosRundataPtr &rdata) = 0;

	virtual AosDataSamplerObjPtr createDataSampler(
							const AosXmlTagPtr &def, 
							const AosRundataPtr &rdata) = 0;
	
	virtual bool registerDataSampler(
							const OmnString &name, 
							const AosDataSamplerObjPtr &cacher) = 0;

	virtual AosDataSamplerObjPtr serializeFrom(
							const AosBuffPtr &buff, 
							const AosRundataPtr &rdata) = 0;
};

#endif

