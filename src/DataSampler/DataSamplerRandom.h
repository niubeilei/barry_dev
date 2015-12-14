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
#ifndef Aos_DataSampler_DataSamplerRandom_h
#define Aos_DataSampler_DataSamplerRandom_h

#include "Conds/Ptrs.h"
#include "SEInterfaces/DataSamplerObj.h"
#include "SEInterfaces/DataProcStatus.h"
#include "Util/DataTypes.h"
#include "DataSampler/DataSampler.h"


class AosValueRslt;

class AosDataSamplerRandom : public AosDataSampler
{
	OmnDefineRCObject;
private:

public:
	AosDataSamplerRandom(const bool flag);
	AosDataSamplerRandom(const AosXmlTagPtr &conf, const AosRundataPtr &rdata);
	~AosDataSamplerRandom();

	virtual bool createSampler(
				const OmnString &fname,
				const int record_len,
				vector<OmnString> &border_values,
				const AosRundataPtr &rdata);

	virtual bool createSampler(
				const AosBuffArrayPtr &orig_array,
				AosBuffArrayPtr &reault_array,
				const AosRundataPtr &rdata);

	virtual AosDataSamplerObjPtr clone();

	virtual AosDataSamplerObjPtr clone(
				const AosXmlTagPtr &conf,
				const AosRundataPtr &rdata);

	virtual bool serializeTo(
				const AosBuffPtr &buff,
				const AosRundataPtr &rdata);

	virtual bool serializeFrom(
				const AosBuffPtr &buff,
				const AosRundataPtr &rdata);
private:
	bool config(const AosXmlTagPtr &def, const AosRundataPtr &rdata);
};

#endif
