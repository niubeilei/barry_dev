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
#ifndef Aos_DataSampler_DataSamplerCreator_h
#define Aos_DataSampler_DataSamplerCreator_h

#include "SEInterfaces/DataSamplerCreatorObj.h"


class AosDataSamplerCreator : virtual public AosDataSamplerCreatorObj
{
public:
	virtual AosDataSamplerObjPtr createStrSampler(const AosRundataPtr &rdata);

	virtual AosDataSamplerObjPtr createDataSampler(
							const AosXmlTagPtr &def, 
							const AosRundataPtr &rdata);
	
	virtual bool registerDataSampler(
							const OmnString &name, 
							const AosDataSamplerObjPtr &field);

	virtual AosDataSamplerObjPtr serializeFrom(
							const AosBuffPtr &buff, 
							const AosRundataPtr &rdata);

	static bool init();

	AosDataSamplerObjPtr createStrField(const AosRundataPtr &rdata);
};

#endif

