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
// 07/23/2012 Created by Felicia
////////////////////////////////////////////////////////////////////////////
#include "DataSampler/DataSampler.h"


AosDataSampler::AosDataSampler(const AosDataSamplerType::E type)
{
	mType = type;
}


AosDataSampler::AosDataSampler(const AosDataSampler &rhs)
{
	mType = rhs.mType;
}


AosDataSampler::~AosDataSampler()
{
}


bool
AosDataSampler::config(const AosXmlTagPtr &def, const AosRundataPtr &rdata)
{
	return true;
}


