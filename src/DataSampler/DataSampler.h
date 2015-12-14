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
#ifndef Aos_DataSampler_DataSampler_h
#define Aos_DataSampler_DataSampler_h

#include "Conds/Ptrs.h"
#include "SEInterfaces/DataSamplerObj.h"
#include "SEInterfaces/DataProcStatus.h"
//#include "SEUtil/SamplerOpr.h"
#include "Util/DataTypes.h"


class AosValueRslt;

class AosDataSampler : virtual public AosDataSamplerObj
{
	OmnDefineRCObject;

protected:

public:
	AosDataSampler(const AosDataSamplerType::E type);
	AosDataSampler(const AosDataSampler &rhs);
	~AosDataSampler();

protected:
	bool config(const AosXmlTagPtr &def, const AosRundataPtr &rdata);
};

#endif
